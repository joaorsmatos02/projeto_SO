/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include <stdio.h>
#include <unistd.h>
#include "../include/configuration.h"
#include "../include/main.h"
#include "../include/mesignal.h"
#include "../include/memory.h"

// variaveis globais de main.c a serem usadas
extern struct main_data* data;
extern int op_counter;
extern struct communication_buffers* buffers;
extern struct semaphores* sems;
extern struct config* configuration;

void print_stats(){
    for(int i = 0; i < op_counter; i++){
        struct operation current = *((data->results) + i); // operacao atual
        if(current.status == 'C'){ // se o pedido tiver terminado
            printf("request:%d status:C start:%ld restaurant:%d rest_time:%ld\n"
                "driver:%d driver_time:%ld client:%d client_end_time:%ld\n",
                i, current.start_time.tv_sec, current.receiving_rest, 
                current.rest_time.tv_sec, current.receiving_driver, current.driver_time.tv_sec,
                current.receiving_client, current.client_end_time.tv_sec);
        } else { // se o pedido ainda nao chegou ao cliente
            printf("request:%d status:%c\n", i, current.status);
        }
    }
    alarm(configuration->alarm_time); // reiniciar alarme
}

void ctrlC() {
    // parar a execucao
    stop_execution(data, buffers, sems);
}