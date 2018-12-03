// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DAINF UTFPR
// Versão 1.0 -- Março de 2015
//
// Estruturas de dados internas do sistema operacional
#include <ucontext.h>

#ifndef __DATATYPES__
#define __DATATYPES__

// Estrutura que define uma tarefa
typedef struct task_t
{
  struct task_t *prev;
  struct task_t *next;
  struct task_t *await;
  ucontext_t context;
  int tid;
  int prio;
  int dynamic_prio;
  int quantum;
  char status;
  unsigned int sleep;
  unsigned int cpu_time;
  unsigned int last_called_time;
  unsigned int activations;
  int system_task;
  int exitCode;
} task_t ;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t ;

#endif
