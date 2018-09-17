#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "datatypes.h"
#include "pingpong.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

ucontext_t ContextMain;
int proximo;

void pingpong_init () {
    setvbuf(stdout, 0, _IONBF, 0);
    proximo = 1;
    //getcontext(&ContextMain);
}

// Cria uma nova tarefa. Retorna um ID> 0 ou erro.
int task_create (task_t *task, void (*start_func)(void *), void *arg) {
    int id = proximo;
    proximo++;
    task->tid = id;
    getcontext(&(task->context));

    char *stack = malloc (STACKSIZE);

    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
    }
    else
    {
        perror ("Erro na criação da pilha: ");
        return -1;
    }

    makecontext (&(task->context), (void *)start_func, 1, arg);

    return task->tid;
}

// Termina a tarefa corrente, indicando um valor de status encerramento
void task_exit (int exitCode) {
}

// alterna a execução para a tarefa indicada
int task_switch (task_t *task) {
    swapcontext(&ContextMain, &(task->context));
    return task->tid;
}

// retorna o identificador da tarefa corrente (main eh 0)
int task_id () {
    return proximo;
}
