/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/synchronization.h"
#include <semaphore.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

sem_t * semaphore_create(char* name, int value) {
    sem_t* semaforo = sem_open(name, O_CREAT, 0, value); // criar semaforo
    if (semaforo == SEM_FAILED){ // se ocorrer erro
        perror("Erro ao criar semaforo");
        exit(0);
    }
    return semaforo;
}

void semaphore_destroy(char* name, sem_t* semaphore) {
    int res = sem_close(semaphore); // fechar semaforo
    if (res == -1) {
        perror("Erro ao fechar semaforo");
        exit(0);
    }
    res = sem_unlink(name); // remover semaforo
        if (res == -1) {
        perror("Erro ao remover semaforo");
        exit(0);
    }
}

void produce_begin(struct prodcons* pc) {
    // bloquear empty e mutex
    semaphore_mutex_lock(pc->empty); 
    semaphore_mutex_lock(pc->mutex);
}

void produce_end(struct prodcons* pc) {
    // desbloquear empty e mutex
    semaphore_mutex_unlock(pc->mutex); 
    semaphore_mutex_unlock(pc->full);
}

void consume_begin(struct prodcons* pc) {
    // bloquear full e mutex
    semaphore_mutex_lock(pc->full); 
    semaphore_mutex_lock(pc->mutex);
}

void consume_end(struct prodcons* pc) {
    // desbloquear full e mutex
    semaphore_mutex_unlock(pc->mutex); 
    semaphore_mutex_unlock(pc->empty); 
}

void semaphore_mutex_lock(sem_t* sem) {
    int res = sem_wait(sem); // fazer wait ao semaforo
    if (res == -1) {
        perror("Erro ao bloquear semaforo");
        exit(0);
    }
}

void semaphore_mutex_unlock(sem_t* sem) {
    int res = sem_post(sem); // fazer post ao semaforo
    if (res == -1) {
        perror("Erro ao desbloquear semaforo");
        exit(0);
    }
}