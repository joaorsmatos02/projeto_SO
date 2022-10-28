/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/process.h"
#include "../include/restaurant.h"
#include "../include/client.h"
#include "../include/driver.h"
#include "../include/main.h"
#include "../include/memory.h"
#include "../include/synchronization.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int launch_restaurant(int restaurant_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if (pid == -1) {
        exit(0);
    } else if(pid == 0){ // processo filho
        int ops = execute_restaurant(restaurant_id, buffers, data, sems);
        exit(ops);
    } else { // processo pai
        return pid;
    }
}

int launch_driver(int driver_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if (pid == -1) {
        exit(0);
    } else if(pid == 0){ // processo filho
        int ops = execute_driver(driver_id, buffers, data, sems);
        exit(ops);
    } else { // processo pai
        return pid;
    }
}

int launch_client(int client_id, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems){
    pid_t pid = fork();
    if (pid == -1) {
        exit(0);
    } else if(pid == 0){ // processo filho
        int ops = execute_client(client_id, buffers, data, sems);
        exit(ops);
    } else { // processo pai
        return pid;
    }
}

int wait_process(int process_id){
    // guardar valor de status em res
    int res;
    waitpid(process_id, &res, 0); 
    // verificar se processo terminou corretamente
    if(WIFEXITED(res)){
        return WEXITSTATUS(res);
    }
    // se o processo não terminou corretemente retorna -1
    return -1;
}