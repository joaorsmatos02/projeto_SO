/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#ifndef STATS_H_GUARD
#define STATS_H_GUARD

#include "main.h"
#include "memory.h"
#include <stdio.h>

/* Funcao que escreve as estatisticas finais de data em file
*/
void write_stats_file(struct main_data* data, FILE* file);

/* Funcao que escreve as estatisticas da operacao fornecida em file
*/
void write_request_statistics(struct operation* op, FILE* file);

#endif