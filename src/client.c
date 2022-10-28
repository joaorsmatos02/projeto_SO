/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/client.h"
#include "../include/main.h"
#include "../include/memory.h"
#include "../include/metime.h"
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>

int execute_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    signal(SIGINT, SIG_IGN); // ignorar sinal CTRL-C
    int operations = 0;
    while(*(data->terminate) != 1){ // ler até ser dada ordem de terminação do programa
        //criar operacao
        struct operation op;
        // ler operação da main
        client_get_operation(&op, client_id, buffers, data, sems);
        if ((op.id) != -1){
            printf("Cliente recebeu pedido!\n");
            // processar operação
            client_process_operation(&op, client_id, data, &operations, sems);
        } else { // se o pedido nao e para este cliente
            produce_end(sems->driv_cli); // notificar outros clientes para lerem do buffer
        }
    }
    return operations;
}

void client_get_operation(struct operation* op, int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    if(*(data->terminate) == 1){
        // terminar se se data->terminate tem valor 1
        return;
    }

    // ler operação do buffer
    consume_begin(sems->driv_cli);
    read_driver_client_buffer(buffers->driv_cli, client_id, data->buffers_size, op);
    consume_end(sems->driv_cli);
}

void client_process_operation(struct operation* op, int client_id, struct main_data* data, int* counter, struct semaphores* sems){
    // registar tempo
    client_register_time(op);
    // alterar receiving client para id
    op->receiving_client = client_id;
    // alterar estado para client
    op->status = 'C';
    // incrementar contador de operações
    (*counter)++;
    // atualizar na main
    sem_wait(sems->results_mutex);
    *(data->results + (op->id)) = *op;
    sem_post(sems->results_mutex);
}