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
task_t *current_task, *main_task, *dispatcher, *task_queue;
int last_task_id;

task_t *scheduler () {

    return task_queue->next;
}

void dispatcher_body () {
    int user_tasks = queue_size((queue_t *) task_queue);

    while (user_tasks > 0) {
        task_t *next = scheduler ();
        if (next) {
            task_switch(next);
            queue_remove((queue_t **) task_queue, (queue_t *) next);
        }
    }

    task_exit(0);
}

void pingpong_init () {
    setvbuf(stdout, 0, _IONBF, 0);
    dispatcher = (task_t *)malloc(sizeof(task_t));
    task_create(dispatcher, dispatcher_body, 0);
    task_queue = NULL;
    last_task_id = 0;

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
    ucontext_t context;
    getcontext (&context);

    last_task_id++;
    task->tid = last_task_id;

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

    queue_append((queue_t **) &task_queue, (queue_t *) task);

    return task->tid;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
   /*
    * Criando uma task_t no init() pra ter referência da main parece ter funcionado
    * Precisa ver com os outros projetos!
   */
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