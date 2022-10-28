/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/restaurant.h"
#include "../include/main.h"
#include "../include/memory.h"
#include "../include/metime.h"
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>

int execute_restaurant(int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    signal(SIGINT, SIG_IGN); // ignorar sinal CTRL-C
    int operations = 0;
    while(*(data->terminate) != 1){ // ler até ser dada ordem de terminação do programa
        //criar operacao
        struct operation op;
        // ler operação da main
        restaurant_receive_operation(&op, rest_id, buffers, data, sems);
        if ((op.id) != -1) {
            printf("Restaurante recebeu pedido!\n");
            // processar operação
            restaurant_process_operation(&op, rest_id, data, &operations, sems);
            // escrever a resposta para o motorista
            restaurant_forward_operation(&op, buffers, data, sems);
        } else { // caso o pedido nao seja para este restaurante
            produce_end(sems->main_rest); // notificar outros restaurantes para ler do buffer
        }
    }
    return operations;
}

void restaurant_receive_operation(struct operation* op, int rest_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    if(*(data->terminate) == 1){
        // terminar se se data->terminate tem valor 1
        return;
    }

    // ler operação do buffer
    consume_begin(sems->main_rest);
    read_main_rest_buffer(buffers->main_rest, rest_id, data->buffers_size, op);
    consume_end(sems->main_rest);
}

void restaurant_process_operation(struct operation* op, int rest_id, struct main_data* data, int* counter, struct semaphores* sems){
    // registar tempo
    rest_register_time(op);
    // alterar receiving restaurant para id
    op->receiving_rest = rest_id;
    // alterar estado para restaurant
    op->status = 'R';
    // incrementar contador de operações
    (*counter)++;
    // atualizar na main
    sem_wait(sems->results_mutex);
    *(data->results + (op->id)) = *op;
    sem_post(sems->results_mutex);
}

void restaurant_forward_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    // escrever operação no buffer entre restaurante e motorista
    produce_begin(sems->rest_driv);
    write_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    produce_end(sems->rest_driv);
}