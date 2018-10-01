#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

void queue_append (queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        printf("A fila não existe");
    }

    if (elem == NULL) {
        printf("O elemento não existe");
    }

    if ((*queue) == NULL) {
        // Primeiro elemento da fila
        elem->next = elem;
        elem->prev = elem;
        *queue = elem;
    }
    else {
        // Esse teste serve para verificar se o elemento pertence a uma fila
        if (elem->prev == NULL && elem->next == NULL) {
            // Primeiro apontamos o prev da cabeça da lista para o novo elemento, que será o último
            (*queue)->prev = elem;
            // Agora vamos testar se já temos mais de um elemento na lista
            queue_t *temp = (*queue)->next;
            if (temp == NULL) {
                (*queue)->next = elem;
                elem->prev = (*queue);
            } else {
                // Vai entrar aqui caso tenha 2 ou mais elementos, precisamos ir até o fim
                while (temp != NULL && temp->next != (*queue)) {
                    temp = temp->next;
                }
                elem->prev = temp;
                temp->next = elem;
            }

            // Sempre apontamos como próximo do novo elemento a cabeça da lista, já que estamos inserindo no final
            elem->next = (*queue);

        }
    }
}

int queue_size (queue_t *queue) {
    int size = 0;

    if (queue == NULL) {
        return size;
    } else {
        for (queue_t *temp = queue; temp != NULL; temp = temp->next) {
            // Foi necessário um if com break, porque se colocar na cláusula de parada do for não dá certo
            if (temp == queue && size > 0) {
                break;
            }
            size++;
        }
    }

    return size;
}

queue_t *queue_remove (queue_t **queue, queue_t *elem) {
    if (queue == NULL) {
        printf("A fila não existe");
    }

    if ((*queue) == NULL) {
        printf("A fila está vazia");
    }

    if (elem == NULL) {
        printf("O elemento não existe");
    }

    int i = 0;
    for (queue_t *temp = (*queue); temp != NULL; temp = temp->next, i++) {
        // Foi necessário um if com break, porque se colocar na cláusula de parada do for não dá certo
        if (temp == (*queue) && i > 0) {
            return NULL;
        }
        if (temp == elem) {
            // Testa se é remoção do único elemento da fila
            if ((*queue) == (*queue)->next && (*queue) == (*queue)->prev) {
                (*queue) = NULL;
            } else {
                // Atualiza as referências para remover o elemento da lista
                queue_t *prev_aux = temp->prev;
                queue_t *next_aux = temp->next;
                prev_aux->next = next_aux;
                next_aux->prev = prev_aux;

                // Testa se é a cabeça da lista
                if ((*queue) == elem) {
                    (*queue) = next_aux;
                }
            }

            elem->prev = NULL;
            elem->next = NULL;

            break;
        }
    }

    return elem;
}

void queue_print (char *name, queue_t *queue, void print_elem (void*) ) {
    printf("%s [", name);

    int i = 0;
    for (queue_t *temp = queue; temp != NULL; temp = temp->next, i++) {
        if (temp == queue && i > 0) {
            break;
        }
        else if (i > 0) {
            printf(" ");
        }
        print_elem(temp);
    }

    printf("]\n");
}