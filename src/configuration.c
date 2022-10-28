/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include <stdio.h>
#include <stdlib.h>
#include "../include/main.h"
#include "../include/configuration.h"

FILE* open_file(char* filename) {
    FILE* file = fopen(filename, "r"); // abrir ficheiro
    if (file == NULL){ // verificar se abriu com sucesso
        perror("Nao e possivel abrir o ficheiro de entrada!\n");
        exit(0);
    }
    return file;
}

FILE* create_file(char* filename) {
    FILE* file = fopen(filename, "w"); // abrir ficheiro
    if (file == NULL){ // verificar se abriu com sucesso
        perror("Nao e possivel criar o ficheiro de entrada!\n");
        exit(0);
    }
    return file;
}

void read_file(FILE* file, struct main_data* data, struct config* configuration) {
    // copiar conteudo para data
    fscanf(file, "%d", &data->max_ops);
    fscanf(file, "%d", &data->buffers_size);
    fscanf(file, "%d", &data->n_restaurants);
    fscanf(file, "%d", &data->n_drivers);
    fscanf(file, "%d", &data->n_clients);

    // criar/abrir ficheiros de log e estatisticas
    char logName[20]; // tamanho max 20 chars
    char statsName[20];
    fscanf(file, "%s", logName); // log_filename
    fscanf(file, "%s", statsName); // stats_filename
    configuration->files->log_file = create_file(logName);
    configuration->files->stats_file = create_file(statsName);

    // registar tempo de alarme
    fscanf(file, "%d", &configuration->alarm_time);
}

void close_file(FILE* file) {
    int res = fclose(file); // fechar ficheiro
    if (res != 0){ // verificar se fechou com sucesso
        perror("Nao e possivel fechar ficheiro de entrada!\n");
        exit(0);
    }
}

void close_log_stats(struct files* files) {
    close_file(files->log_file);
    close_file(files->stats_file);
}