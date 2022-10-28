/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/driver.h"
#include "../include/main.h"
#include "../include/memory.h"
#include "../include/metime.h"
#include <stdio.h>
#include <signal.h>
#include <semaphore.h>

int execute_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    signal(SIGINT, SIG_IGN); // ignorar sinal CTRL-C
    int operations = 0;
    while(*(data->terminate) != 1){ // ler até ser dada ordem de terminação do programa
        //criar operacao
        struct operation op;
        // ler operação da main
        driver_receive_operation(&op, buffers, data, sems);
        if ((op.id) != -1){
            printf("Motorista recebeu pedido!\n");
            // processar operação
            driver_process_operation(&op, driver_id, data, &operations, sems);
            // escrever a resposta para os clientes
            driver_send_answer(&op, buffers, data, sems);
        }
    }
    return operations;
}

void driver_receive_operation(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    if(*(data->terminate) == 1){
        // terminar se se data->terminate tem valor 1
        return;
    }

    // ler operação do buffer
    consume_begin(sems->rest_driv);
    read_rest_driver_buffer(buffers->rest_driv, data->buffers_size, op);
    consume_end(sems->rest_driv);
}

void driver_process_operation(struct operation* op, int driver_id, struct main_data* data, int* counter, struct semaphores* sems){
    // registar tempo
    driver_register_time(op);
    // alterar receiving driver para id
    op->receiving_driver = driver_id;
    // alterar estado para driver
    op->status = 'D';
    // incrementar contador de operações
    (*counter)++;
    // atualizar na main
    sem_wait(sems->results_mutex);
    *(data->results + (op->id)) = *op;
    sem_post(sems->results_mutex);
}

void driver_send_answer(struct operation* op, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    // escrever operação no buffer entre motorista e cliente
    produce_begin(sems->driv_cli);
    write_driver_client_buffer(buffers->driv_cli, data->buffers_size, op);
    produce_end(sems->driv_cli);
}