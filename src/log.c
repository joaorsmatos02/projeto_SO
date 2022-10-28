/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "../include/log.h"

void write_time(FILE* file) {
    // criar string
    char buffer[23];
    // criar estruturas
    struct timespec ts;
    struct tm t;
    clock_gettime(CLOCK_REALTIME, &ts); // obter hora atual
    localtime_r(&(ts.tv_sec), &t); // formatar resultado
    strftime(buffer, 23, "%F %T", &t); // escrever data e hora
    snprintf(&buffer[19], 5, ".%ld", ts.tv_nsec); // escrever milissegundos
    // escrever no ficheiro
    fprintf(file, "%s", buffer);
}

void write_request(FILE* file) {
    // escrever hora
    write_time(file);
    // escrever request
    fprintf(file, "%s", " request\n");
}

void write_status(int num, FILE* file){
    // escrever hora
    write_time(file);
    // escrever status
    fprintf(file, "%s", " status ");

    // escrever numero do pedido
    char req[5];
    sprintf(req, "%d", num);
    strcat(req, "\n");
    fprintf(file, "%s", req);
}

void write_stop(FILE* file) {
    // escrever hora
    write_time(file);
    // escrever stop
    fprintf(file, "%s",  " stop\n");
}

void write_help(FILE* file) {
    // escrever hora
    write_time(file);
    // escrever help
    fprintf(file, "%s", " help\n");
}