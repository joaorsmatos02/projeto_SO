/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/stats.h"
#include "../include/main.h"
#include "../include/memory.h"
#include <stdio.h>
#include <time.h>

extern int op_counter;

void write_stats_file(struct main_data* data, FILE* file) {
    fprintf(file, "%s", "Process Statistics:\n");
    
    /* escrever estatisticas gerais */
    
    // restaurantes
    for(int i = 0; i < data->n_restaurants; i++) {
        fprintf(file, "\tRestaurant %d prepared %d requests!\n", i, *(data->restaurant_stats + i));
    } 

    // motoristas
    for(int i = 0; i < data->n_drivers; i++) {
        fprintf(file, "\tDriver %d delivered %d requests!\n", i, *(data->driver_stats + i));
    } 

    // clientes
    for(int i = 0; i < data->n_clients; i++) {
        fprintf(file, "\tClient %d recieved %d requests!\n", i, *(data->client_stats + i));
    }

    fprintf(file, "%s", "\n");

    /* escrever estatisticas de cada pedido */

    for(int i = 0; i < op_counter; i++) {
        write_request_statistics(data->results + i, file);
        if(i != op_counter - 1)
            printf("\n");
    }
}

void write_request_statistics(struct operation* op, FILE* file) {
    // obter datas
    char created[23];
    char restaurant[23];
    char driver[23];
    char client[23];

    struct tm t;

    localtime_r(&(op->start_time.tv_sec), &t); // formatar resultado
    strftime(created, 23, "%F %T", &t); // escrever data e hora
    snprintf(&created[19], 5, ".%ld", op->start_time.tv_nsec); // escrever milissegundos;

    localtime_r(&(op->rest_time.tv_sec), &t); // formatar resultado
    strftime(restaurant, 23, "%F %T", &t); // escrever data e hora
    snprintf(&restaurant[19], 5, ".%ld", op->rest_time.tv_nsec); // escrever milissegundos;

    localtime_r(&(op->driver_time.tv_sec), &t); // formatar resultado
    strftime(driver, 23, "%F %T", &t); // escrever data e hora
    snprintf(&driver[19], 5, ".%ld", op->driver_time.tv_nsec); // escrever milissegundos;

    localtime_r(&(op->client_end_time.tv_sec), &t); // formatar resultado
    strftime(client, 23, "%F %T", &t); // escrever data e hora
    snprintf(&client[19], 5, ".%ld", op->client_end_time.tv_nsec); // escrever milissegundos;

    // calcular tempo total
    double total = ((double)(op->client_end_time.tv_nsec - op->start_time.tv_nsec)) / 1000000000;

    // escrever no ficheiro
    fprintf(file, "Request Statistics:\n"
                  "Request: %d\n"
                  "Status: %c\n"
                  "Restaurant id: %d\n"
                  "Driver id: %d\n"
                  "Client id: %d\n"
                  "Created: %s\n"
                  "Restaurant time: %s\n"
                  "Driver time: %s\n"
                  "Client time (end): %s\n"
                  "Total Time: %0.3f\n\n", op->id, op->status, op->receiving_rest, op->receiving_driver, op->receiving_client, created, restaurant, driver, client, total);
}