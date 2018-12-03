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
  char semaphore;
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
  task_t *semaphore_queue;
  task_t *current_task;
  //struct mutex_t *lock;
  int counter;
} semaphore_t ;

// estrutura que define um mutex
typedef struct
{
  task_t *owner;
  int lock;
  //struct semaphore_t *semaphore;
} mutex_t ;

// estrutura que define uma barreira
typedef struct
{
  int size;
  int tasks;
  semaphore_t semaphore;
  task_t *await;
} barrier_t ;

// estrutura que define uma fila de mensagens
typedef struct
{
  int max;
  int size;
  void *body;
} mqueue_t ;

#endif
