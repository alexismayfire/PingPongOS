// Carlos Alexandre Leite Vieira
// Matheus Mattos Almeida Cruz
//
// Para executar com prioridades, o programa deve ser compilado com a flag -DPRIORITY
//
// Teste da contabilização - tarefas com prioridades distintas

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>

#include "queue.h"
#include "datatypes.h"
#include "pingpong.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define QUANTUM 20;

ucontext_t ContextMain;
task_t *current_task, *main_task, *dispatcher, *ready_queue, *suspended_queue, *ready_tasks;
int last_task_id, alfa_aging = -1;
unsigned int clock = 0;

struct sigaction action;
struct itimerval timer;

void signal_handler () {
    clock++;
    // Se for tarefa de sistema, não faz nada!
    if (current_task->system_task == 0) {
        // Se a tarefa ainda tem quantum, só decrementa; se zerou, volta pro dispatcher
        current_task->quantum--;
        if (current_task->quantum == 0) {
            // Incrementa o CPU time quando a tarefa recebe o processador
            current_task->cpu_time += (systime() - current_task->last_called_time);
            dispatcher->last_called_time = systime();
            task_yield();
        }
    }
}

#ifdef PRIORITY
task_t *scheduler () {
    task_t *next = ready_queue, *temp;

    // Se remover o laço abaixo e apenas ir removendo da lista, o resultado sempre é o esperado
    // Porém, quando o laço é usado, podem ter algumas distorções no resultado
    // Mesmo usando um DEFINE DEBUG e usando um condicional aqui para pular não adianta, precisa comentar (?)

    int size = 0;
    for (temp = ready_queue; temp != NULL; temp = temp->next) {
        // Foi necessário um if com break, porque se colocar na cláusula de parada do for não dá certo
        if (temp == ready_queue && size > 0) {
            break;
        }
        if (temp->dynamic_prio <= next->dynamic_prio) {
            if ((temp->dynamic_prio == next->dynamic_prio && temp->tid < next->tid)
                    || temp->dynamic_prio < next->dynamic_prio) {
                // Atualiza a tarefa que seria escolhida até então,
                // quando as tarefas tem a mesma prioridade, a com menor ID ganha
                next->dynamic_prio += alfa_aging;
                next = temp;
            }
        } else {
            // Se a tarefa não foi escolhida, aumenta a prioridae
            temp->dynamic_prio += alfa_aging;
        }
        size++;
    }

    queue_remove((queue_t **) &ready_queue, (queue_t *) next);
    queue_append((queue_t **) &ready_queue, (queue_t *) next);
    next->dynamic_prio = next->prio;
    return next;
}
#else
task_t *scheduler () {
    task_t *next = ready_queue;

    queue_remove((queue_t **) &ready_queue, (queue_t *) next);
    queue_append((queue_t **) &ready_queue, (queue_t *) next);

    return next;
}
#endif

void dispatcher_body () {
    int user_tasks = queue_size((queue_t *) ready_queue);

    while (user_tasks > 0) {
        task_t *next = scheduler ();

        if (next) {
            // Se uma tarefa for removida, vamos colocar ela no final da fila
            //queue_append((queue_t **) &ready_queue, (queue_t *) next);
            task_switch(next);
        }
        user_tasks = queue_size((queue_t *) ready_queue);
    }

    task_exit(0);
}

void pingpong_init () {
    setvbuf(stdout, 0, _IONBF, 0);

    /* Dispatcher e lista de tarefas */
    dispatcher = (task_t *)malloc(sizeof(task_t));
    task_create(dispatcher, dispatcher_body, 0);
    dispatcher->system_task = 1;
    dispatcher->tid = 1;
    ready_queue = NULL;
    suspended_queue = NULL;
    ready_tasks = NULL;

    last_task_id = 0;

    /* Inicializa a main */
    current_task = (task_t *)malloc(sizeof(task_t));
    task_create(current_task, task_yield, 0);
    main_task = current_task;

    last_task_id = 2;

    /* Inicializa o temporizador */
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGALRM, &action, 0) < 0) {
        perror("Erro em sigaction: ");
        exit(1);
    }

    timer.it_value.tv_usec = 1000; // primeiro disparo, em micro-segundos
    timer.it_interval.tv_usec = 1000; // disparos subsequentes, em micro-segundos

    if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
        perror("Erro em setitimer: ");
        exit(1);
    }

    task_yield();
}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task, void (*start_func)(void *), void *arg) {
    task->tid = last_task_id;
    last_task_id++;

    /* Inicializa um novo contexto para a tarefa */
    ucontext_t context;
    getcontext (&context);
    char *stack = malloc (STACKSIZE);
    if (stack)
    {
        context.uc_stack.ss_sp = stack;
        context.uc_stack.ss_size = STACKSIZE;
        context.uc_stack.ss_flags = 0;
        context.uc_link = 0;
    }
    else
    {
        perror ("Erro na criação da pilha: ");
        return -1;
    }

    makecontext (&context, (void *)start_func, 1, arg);
    task->context = context;

    /* Inicializa os outros atributos */
    task->prio = 0;
    task->dynamic_prio = 0;
    task->activations = 0;
    task->cpu_time = 0;
    task->system_task = 0; // tarefas de usuário são sempre zero
    task->status = 'r'; // a priori, todas as tasks tem o estado 'ready'
    task->exitCode = -1;

    queue_append((queue_t **) &ready_queue, (queue_t *) task);

    return task->tid;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
    current_task->exitCode = exitCode;

    printf("Task %d exit: running time %u ms, cpu time  %u ms, %u activations\n",
           current_task->tid, systime(),
           current_task->cpu_time, current_task->activations
    );

    // Se a tarefa saiu com código 0, podemos remover da lista
    if (0 == current_task->system_task) {
        queue_remove((queue_t **) &ready_queue, (queue_t *) current_task);

        task_t *temp, *first_sleeping = suspended_queue;
        int size = 0;
        for (temp = suspended_queue; temp != NULL; temp = temp->next) {
            if (temp == first_sleeping && size > 0) {
                break;
            }

            if (temp->await == current_task) {
                task_resume(temp);
                task_yield();
            }
            size++;
        }
        /*
        // Acorda as tarefas aguardando, iterando na fila de sleeping
        task_t *temp, *join_temp;
        int size = 0;
        for (temp = suspended_queue; temp != NULL; temp = temp->next) {
            // Foi necessário um if com break, porque se colocar na cláusula de parada do for não dá certo
            if (temp == suspended_queue && size > 0) {
                break;
            }
            // Agora, precisa saber se a tarefa da fila está aguardando a tarefa que encerrou
            task_t *first_join = temp->join_queue;
            int size_join = 0;
            for (join_temp = temp->join_queue; join_temp != NULL; join_temp = join_temp->next) {
                if (join_temp == first_join && size > 0) {
                    break;
                }
                if (join_temp->tid == current_task->tid) {
                    task_resume(temp);
                    queue_remove((queue_t **) temp->join_queue, (queue_t *) current_task);
                    break;
                }
                size_join++;
            }
            size++;
        }
         */

        task_yield();
    }

    // Se não foi uma tarefa de usuário, volta pra main
    task_switch(main_task);
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {
    task_t *temp;
    temp = current_task;

    // Se for uma tarefa de usuário, inicio ela com o quantum padrão
    if (task->system_task == 0) {
        task->quantum = QUANTUM;
        task->last_called_time = systime();
        task->activations++;
    }

    if (temp->system_task == 1) {
        temp->cpu_time += (systime() - temp->last_called_time);
    }

    current_task = task;
    swapcontext(&(temp->context), &(current_task->context));

    return task->tid;
}

// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    return current_task->tid;
}

void task_yield () {
    dispatcher->activations++;
    task_switch(dispatcher);
}

int task_join (task_t *task) {
    if (task == NULL) {
        // Se a tarefa não existir ou já tiver sido encerrada, retorna imediatamente
        return -1;
    }

    current_task->await = task;
    task_suspend(NULL, &suspended_queue);

    return task->exitCode;
}

void task_suspend (task_t *task, task_t **queue) {
    if (task == NULL) {
        task = current_task;
    }
    task->status = 's';
    queue_remove((queue_t **) &ready_queue, (queue_t *) task);
    // Para manter o controle de tarefas aguardando
    queue_append((queue_t **) queue, (queue_t *) task);

    task_yield();
}

void task_resume (task_t *task) {
    // Para manter o controle de tarefas aguardando
    queue_remove((queue_t **) &suspended_queue, (queue_t *) task);
    task->status = 'r';
    queue_append((queue_t **) &ready_queue, (queue_t *) task);
}

void task_setprio (task_t *task, int prio) {
    // Se a tarefa for nula, deve setar a prioridade da tarefa corrente
    if (task == NULL) {
        current_task->prio = prio;
        current_task->dynamic_prio = prio;
    } else {
        task->prio = prio;
        task->dynamic_prio = prio;
    }
}

int task_getprio (task_t *task) {
    // Se a tarefa for nula, retornar a prioridade da tarefa atual
    if (task != NULL) {
        return task->prio;
    } else {
        return current_task->prio;
    }
}

unsigned int systime () {
    return clock;
}