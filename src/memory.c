/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/memory.h"
#include "../include/main.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>

void* create_shared_memory(char* name, int size){
    // criar zona de memoria
    int shm = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm == -1){
        perror("shm_open");
        exit(0);
    }
    // definir tamanho
    int ret = ftruncate(shm, size);
    if (ret == -1){
        perror("ftruncate");
        exit(0);
    }
    // mapear no espaço de endereçamento virtual
    int *ptr = mmap(0, size, PROT_READ|PROT_WRITE,MAP_SHARED,shm,0); 
    if (ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }
    // mapear 0
    bzero(ptr, size); 
    return ptr;
}

void* create_dynamic_memory(int size){
    // criar memoria dinamica
    int* ret = malloc(size);
    if (ret == NULL){
        perror("malloc");
        exit(0);
    }
    return ret;
}

void destroy_shared_memory(char* name, void* ptr, int size){
    // terminar projeçao de memoria
    int ret = munmap(ptr, size);
    if (ret == -1){
        perror("munmap");
        exit(0);
    }
    // eliminar zona de memoria
    ret = shm_unlink(name);
    if (ret == -1){
        perror("shm_unlink");
        exit(0);
    }
}

void destroy_dynamic_memory(void* ptr){
    // eliminar memoria dinamica
    free(ptr);
}

void write_main_rest_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    int success = 0;
    for(int i = 0; i < buffer_size && !success; i++){ // percorrer buffer
        if(*((buffer->ptrs)+i) == 0){ // se posicao estiver vazia
            *((buffer->buffer)+i) = *op; // escrever no buffer
            success = 1;
            *((buffer->ptrs)+i)=1; // marcar posicao como ocupada
        }
    }
}

void write_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    if(!(((buffer->ptrs->in) + 1) % buffer_size == (buffer->ptrs->out))){
        // vericar se out está na posicao a seguir a in
        // para evitar escrever sobre posicoes ainda nao lidas
        // salvaguarda-se 1 posicao vazia
        *((buffer->buffer)+(buffer->ptrs->in)) = *op; // escrever no buffer
        // passar o apontador de escrita a proxima posicao
        (buffer->ptrs->in) = ((buffer->ptrs->in) + 1) % buffer_size;
    }
}

void write_driver_client_buffer(struct rnd_access_buffer* buffer, int buffer_size, struct operation* op) {
    int success = 0;
    for(int i = 0; i < buffer_size && !success; i++){ // percorrer buffer
        if(*((buffer->ptrs)+i) == 0){ // se posicao estiver vazia
            *((buffer->buffer)+i) = *op; // escrever no buffer
            success = 1;
            *((buffer->ptrs)+i)=1; // marcar posicao como ocupada
        }
    }
}

void read_main_rest_buffer(struct rnd_access_buffer* buffer, int rest_id, int buffer_size, struct operation* op) {
    // percorrer buffer
    for(int i = 0; i < buffer_size; i++){
        // verificar se esta ocupado e se tem o mesmo id
        if(*((buffer->ptrs)+i) == 1 && ((buffer->buffer)+i)->requested_rest == rest_id){
            // se sim copiar informacao
            *op = *((buffer->buffer)+i);
            // marcar posicao como livre
            *((buffer->ptrs)+i) = 0;
            return;
        }
    }
    op->id = -1; // se nao houver pedidos para restaurante
}

void read_rest_driver_buffer(struct circular_buffer* buffer, int buffer_size, struct operation* op) {
    if(buffer->ptrs->in == buffer->ptrs->out){
        op->id = -1; // se nao houver pedidos a leitura e invalida
        return;
    }
    // copiar informacoes
    *op = *((buffer->buffer)+(buffer->ptrs->out));
    // incrementar apontador de leitura
    (buffer->ptrs->out) = (buffer->ptrs->out + 1) % buffer_size;
}

void read_driver_client_buffer(struct rnd_access_buffer* buffer, int client_id, int buffer_size, struct operation* op) {
    // percorrer buffer
    for(int i = 0; i < buffer_size; i++){
        // verificar se esta ocupado e se tem o mesmo id
        if(*((buffer->ptrs)+i) == 1 && ((buffer->buffer)+i)->requesting_client == client_id){
            // se sim copiar informacao
            *op = *((buffer->buffer)+i);
            // marcar posicao como livre
            *((buffer->ptrs)+i) = 0;
            return;
        }
    }
    op->id = -1; // se nao houver pedidos para restaurante
}