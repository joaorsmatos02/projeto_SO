/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#ifndef METIME_H_GUARD
#define METIME_H_GUARD

#include "memory.h"

/* Função que regista o tempo atual no campo do tempo inicial na
* estrutura fornecida
*/
void start_register_time(struct operation* op);

/* Função que regista o tempo atual no campo do restaurante na
* estrutura fornecida
*/
void rest_register_time(struct operation* op);

/* Função que regista o tempo atual no campo do motorista na
* estrutura fornecida
*/
void driver_register_time(struct operation* op);

/* Função que regista o tempo atual no campo do cliente na
* estrutura fornecida
*/
void client_register_time(struct operation* op);

#endif