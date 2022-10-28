/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#ifndef LOG_H_GUARD
#define LOG_H_GUARD

/* Função que escreve a data e hora atuais no ficheiro fornecido
*/
void write_time(FILE* file);

/* Função que escreve a operação de request no ficheiro file
*/
void write_request(FILE* file);

/* Função que escreve a operação de status do pedido num no ficheiro file
*/
void write_status(int num, FILE* file);

/* Função que escreve a operação de stop no ficheiro file
*/
void write_stop(FILE* file);

/* Função que escreve a operação de help no ficheiro file
*/
void write_help(FILE* file);

#endif