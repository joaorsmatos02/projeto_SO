/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/metime.h"
#include "../include/memory.h"
#include <time.h>

void start_register_time(struct operation* op) {
    clock_gettime(CLOCK_REALTIME, &(op->start_time));
}

void rest_register_time(struct operation* op) {
    clock_gettime(CLOCK_REALTIME, &(op->rest_time));
}

void driver_register_time(struct operation* op) {
    clock_gettime(CLOCK_REALTIME, &(op->driver_time));
}

void client_register_time(struct operation* op) {
    clock_gettime(CLOCK_REALTIME, &(op->client_end_time));
}