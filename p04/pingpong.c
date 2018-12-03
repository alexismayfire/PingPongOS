#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/signal.h>

#include "queue.h"
#include "datatypes.h"
#include "pingpong.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

ucontext_t ContextMain;
task_t *current_task, *main_task, *dispatcher, *ready_queue, *ready_tasks;
int last_task_id, alfa_aging = -1;

task_t *scheduler () {
    task_t *next = ready_queue, *temp;
    int size = 0;
    for (temp = ready_queue; temp != NULL; temp = temp->next) {
        // Foi necessário um if com break, porque se colocar na cláusula de parada do for não dá certo
        if (temp == ready_queue && size > 0) {
            break;
        }
        if (temp->dynamic_prio <= next->dynamic_prio) {
            if ((temp->dynamic_prio == next->dynamic_prio && temp->tid < next->tid)
                || temp->dynamic_prio < next->dynamic_prio) {
                // Atualiza a tarefa que seria escolhida até então
                next->dynamic_prio += alfa_aging;
                next = temp;
            }
        } else {
            temp->dynamic_prio += alfa_aging;
        }
        size++;
    }

    queue_remove((queue_t **) &ready_queue, (queue_t *) next);
    next->dynamic_prio = next->prio;

    return next;
}

void dispatcher_body () {
    int user_tasks = queue_size((queue_t *) ready_queue);

    while (user_tasks > 0) {
        task_t *next = scheduler ();
        if (next) {
            // Se uma tarefa for removida, vamos colocar ela no final da fila
            queue_append((queue_t **) &ready_queue, (queue_t *) next);
            task_switch(next);
        }
        user_tasks = queue_size((queue_t *) ready_queue);
    }

    task_exit(0);
}

void pingpong_init () {
    setvbuf(stdout, 0, _IONBF, 0);
    last_task_id = 0;

    dispatcher = (task_t *)malloc(sizeof(task_t));
    task_create(dispatcher, dispatcher_body, 0);
    ready_queue = NULL;
    ready_tasks = NULL;

    getcontext(&ContextMain);

    char *stack = malloc (STACKSIZE);
    if (stack)
    {
        ContextMain.uc_stack.ss_sp = stack;
        ContextMain.uc_stack.ss_size = STACKSIZE;
        ContextMain.uc_stack.ss_flags = 0;
        ContextMain.uc_link = 0;
    }

    current_task = (task_t *)malloc(sizeof(task_t));
    current_task->context = ContextMain;
    main_task = current_task;
}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task, void (*start_func)(void *), void *arg) {
    last_task_id++;
    task->tid = last_task_id;

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
    task->prio = 0;
    task->dynamic_prio = task->prio;
    task->exitCode = -1;

    queue_append((queue_t **) &ready_queue, (queue_t *) task);

    return task->tid;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
   /*
    * Criando uma task_t no init() pra ter referência da main parece ter funcionado
    * Precisa ver com os outros projetos!
   */
   current_task->exitCode = exitCode;
   /*
    * Aqui estou forçando uma verificação de tid (dispatcher sempre 1)
    * Precisa pensar em outra maneira mais coerente
    */
   if (0 == exitCode && current_task->tid > 1) {
       queue_remove((queue_t **) &ready_queue, (queue_t *) current_task);
       task_yield();
   }

   task_switch(main_task);
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {
    /*
    task_t *tarefa_atual;
    getcontext(&ContextMain);
    // Aqui agora dá segmentation fault... mas não fica em loop eterno
    tarefa_atual->context = ContextMain;
    swapcontext(&(tarefa_atual->context), &(task->context));
    */
    // Agora funciona o teste1, mas o 2 e o 3 só executa a última
    task_t *temp;
    temp = current_task;
    // Vamos testar se a tarefa encerrou para remover da fila
    /*
    if (current_task->exitCode == 0) {
        queue_remove((queue_t **) ready_queue, (queue_t *) current_task);
    }
     */
    current_task = task;
    swapcontext(&(temp->context), &(current_task->context));

    return task->tid;
}

// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    return current_task->tid;
}

void task_yield () {
    task_switch(dispatcher);
}

void task_setprio (task_t *task, int prio) {
    task->prio = prio;
    task->dynamic_prio = task->prio;
}

int task_getprio (task_t *task) {
    if (task != NULL) {
        return task->prio;
    } else {
        return current_task->prio;
    }
}