/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#ifndef CONFIGURATION_H_GUARD
#define CONFIGURATION_H_GUARD

#include <stdio.h>
#include "main.h"

/* Estrutura que agrega os ficheiros a escrever durante a execucao do magnaeats
*/
struct files {
    FILE* log_file;     // ficheiro de log
    FILE* stats_file;   // ficheiro de estatisticas
};

/* Estrutura que agrega os ficheiros e o tempo de alarme
*/
struct config {
    struct files* files; // ficheiros
    int alarm_time;      // tempo do alarme
};

/* Funcao que abre o ficheiro com o nome passado em filename e retorna
* um apontador para o mesmo
*/
FILE* open_file(char* filename);

/* Funcao que cria o ficheiro com o nome passado em filename e retorna
* um apontador para o mesmo
*/
FILE* create_file(char* filename);

/* Funcao que lê o ficheiro file e copia o conteudo relevante para data e configuration
*/
void read_file(FILE* file, struct main_data* data, struct config* configuration);

/* Funcao que fecha o ficheiro file
*/
void close_file(FILE* file);

/* Funcao que fecha os ficheiros na estrutura fornecida
*/
void close_log_stats(struct files* files);

#endif