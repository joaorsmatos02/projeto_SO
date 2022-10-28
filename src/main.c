/*
* Grupo SO-020
* Daniel Luís nº 56362
* João Matos nº 56292
* João Santos nº 57103
*/

#include "../include/main.h"
#include "../include/driver.h"
#include "../include/client.h"
#include "../include/restaurant.h"
#include "../include/memory.h"
#include "../include/process.h"
#include "../include/semaphore.h"
#include "../include/configuration.h"
#include "../include/log.h"
#include "../include/mesignal.h"
#include "../include/metime.h"
#include "../include/stats.h"
#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int op_counter;                 // variavel global que conta as operacoes realizadas
struct config* configuration;    // estrutura global que agrega os nomes dos ficheiros a escrever e o tempo de alarme
struct main_data* data;
struct communication_buffers* buffers;
struct semaphores* sems;
int stop;                       // variavel que determina o fim da execucao


int main(int argc, char *argv[]) {
    //init data structures
    data = create_dynamic_memory(sizeof(struct main_data));
    buffers = create_dynamic_memory(sizeof(struct communication_buffers));
    buffers->main_rest = create_dynamic_memory(sizeof(struct rnd_access_buffer));
    buffers->rest_driv = create_dynamic_memory(sizeof(struct circular_buffer));
    buffers->driv_cli = create_dynamic_memory(sizeof(struct rnd_access_buffer));

    //init semaphore data structure
    sems = create_dynamic_memory(sizeof(struct semaphores));
    sems->main_rest = create_dynamic_memory(sizeof(struct prodcons));
    sems->rest_driv = create_dynamic_memory(sizeof(struct prodcons));
    sems->driv_cli = create_dynamic_memory(sizeof(struct prodcons));

    //init config structure
    configuration = create_dynamic_memory(sizeof(struct config));
    configuration->files = create_dynamic_memory(sizeof(struct files));

    //execute main code
    main_args(argc, argv, data);
    create_dynamic_memory_buffers(data);
    create_shared_memory_buffers(data, buffers);
    create_semaphores(data, sems);
    launch_processes(buffers, data, sems);
    user_interaction(buffers, data, sems);

    //release memory before terminating
    destroy_dynamic_memory(data);
    destroy_dynamic_memory(buffers->main_rest);
    destroy_dynamic_memory(buffers->rest_driv);
    destroy_dynamic_memory(buffers->driv_cli);
    destroy_dynamic_memory(buffers);
    destroy_dynamic_memory(sems->main_rest);
    destroy_dynamic_memory(sems->rest_driv);
    destroy_dynamic_memory(sems->driv_cli);
    destroy_dynamic_memory(sems);
    destroy_dynamic_memory(configuration->files);
    destroy_dynamic_memory(configuration);
}

void main_args(int argc, char* argv[], struct main_data* data) {
    if(argc == 2){
        // abrir ficheiro de entrada
        FILE* file = open_file(argv[1]);
        // ler ficheiro
        read_file(file, data, configuration);
        // fechar ficheiro
        close_file(file);
        // verificar se argumentos sao validos
        if((data->max_ops) < 1 || (data->buffers_size) < 1 || (data->n_restaurants) < 1 || (data->n_drivers) < 1 || (data->n_clients) < 1) {
            printf("Parametros incorretos!\n");
            exit(0);
        }
    } else { // se não tiver sido passado 1 argumento
        printf("Numero incorreto de parametros!\n");
        exit(0);
    }
}

void create_dynamic_memory_buffers(struct main_data* data) {
    // criar memoria dinamica para restaurantes
    data->restaurant_pids = create_dynamic_memory((data->n_restaurants)*sizeof(int));
    data->restaurant_stats = create_dynamic_memory((data->n_restaurants)*sizeof(int));

    // criar memoria dinamica para motoristas
    data->driver_pids = create_dynamic_memory((data->n_drivers)*sizeof(int));
    data->driver_stats = create_dynamic_memory((data->n_drivers)*sizeof(int));

    // criar memoria dinamica para clientes
    data->client_pids = create_dynamic_memory((data->n_clients)*sizeof(int));
    data->client_stats = create_dynamic_memory((data->n_clients)*sizeof(int));
}

void create_shared_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    // reservar memoria para os buffers de communication_buffers
    (buffers->main_rest)->buffer = create_shared_memory(STR_SHM_MAIN_REST_BUFFER, data->buffers_size);
    (buffers->rest_driv)->buffer = create_shared_memory(STR_SHM_REST_DRIVER_BUFFER, data->buffers_size);
    (buffers->driv_cli)->buffer = create_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, data->buffers_size);

    // reservar memoria para os pointers de communication_buffers
    (buffers->main_rest)->ptrs = create_shared_memory(STR_SHM_MAIN_REST_PTR, sizeof(int));
    (buffers->rest_driv)->ptrs = create_shared_memory(STR_SHM_REST_DRIVER_PTR, sizeof(struct pointers));
    (buffers->driv_cli)->ptrs = create_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, sizeof(int));

    // reservar memoria para data->results
    data->results = create_shared_memory(STR_SHM_RESULTS, (data->max_ops)*sizeof(struct operation));

    // reservar memoria para data->terminate
    data->terminate = create_shared_memory(STR_SHM_TERMINATE, sizeof(int));
}

void launch_processes(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // iniciar processos de restaurantes
    for(int i = 0; i < (data->n_restaurants); i++){
        *(data->restaurant_pids + i) = launch_restaurant(i, buffers, data, sems);
    }

    // iniciar processos de motoristas
    for(int i = 0; i < (data->n_drivers); i++){
        *(data->driver_pids + i) = launch_driver(i, buffers, data, sems);
    }
    
    // iniciar processos de clientes
    for(int i = 0; i < (data->n_clients); i++){
        *(data->client_pids + i) = launch_client(i, buffers, data, sems);
    }
}

void user_interaction(struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    //preparar para receber sinais
    struct sigaction sa;
    sa.sa_handler = ctrlC;
    sa.sa_flags = 0;
    sigaddset(&sa.sa_mask, SIGALRM); // ignorar alarme enquanto trata ctrl-c
    sigaction(SIGINT, &sa, NULL);

    signal(SIGALRM, print_stats);
    alarm(configuration->alarm_time);

    printf("Ações disponíveis:\n"
                        "\trequest client restaurant dish - criar um novo pedido\n"
                        "\tstatus id - consultar o estado de um pedido\n"
                        "\tstop - termina a execução do magnaeats.\n"
                        "\thelp - imprime informação sobre as ações disponíveis.\n");
    stop = 0;

    while (!stop) { // enquanto nao escolher stop
        printf("Introduzir ação:\n");
        char line[7];
        scanf("%s", line); // ler apenas ate ao primeiro espaco
        if(stop == 0){ // verificar se foi dada ordem de terminacao enquanto esperava por input
            // verificar qual a opcao escolhida
            if (strcmp(line, "request") == 0) {
                create_request(&op_counter, buffers, data, sems);
            } else if (strcmp(line, "status") == 0) {
                read_status(data, sems);
            } else if (strcmp(line, "stop") == 0) {
                stop_execution(data, buffers, sems);
            } else if (strcmp(line, "help") == 0) {
                write_help(configuration->files->log_file); // escrever no log
                printf("Ações disponíveis:\n"
                        "\trequest client restaurant dish - criar um novo pedido\n"
                        "\tstatus id - consultar o estado de um pedido\n"
                        "\tstop - termina a execução do magnaeats.\n"
                        "\thelp - imprime informação sobre as ações disponíveis.\n");
            } else {
                printf("Ação não reconhecida, insira 'help' para assistência.\n");
            }
        }
    }
}

void create_request(int* op_counter, struct communication_buffers* buffers, struct main_data* data, struct semaphores* sems) {
    // ler id e nome
    int cli, rest;
    scanf("%d", &cli);
    scanf("%d", &rest);
    char dish[20]; // tamanho maximo 20 caracteres
    scanf("%s", dish);
    if(*op_counter >= data->max_ops){ // verificar se o numero maximo de ops foi excedido
        printf("O numero maximo de pedidos foi alcançado!\n");
    } else if(cli > data->n_clients-1) { // verificar se id de cliente é valido
        printf("Id de cliente invalido!\n");
    } else if (rest > data->n_restaurants-1) { // verificar se id de restaurante é valido
        printf("Id de restaurante invalido!\n");
    } else {
        // escrever no log
        write_request(configuration->files->log_file);
        // guardar na main
        (data->results + *op_counter)->id = *op_counter;
        (data->results + *op_counter)->requested_rest = rest;
        (data->results + *op_counter)->requesting_client = cli;
        (data->results + *op_counter)->status = 'I';
        // reservar memoria para nome do prato
        (data->results + *op_counter)->requested_dish = create_dynamic_memory(sizeof(char[20]));
        strcpy((data->results + *op_counter)->requested_dish, dish);
        start_register_time(data->results + *op_counter); // registar tempo

        // escrever no buffer
        produce_begin(sems->main_rest);
        write_main_rest_buffer(buffers->main_rest, data->buffers_size, (data->results + *op_counter));
        produce_end(sems->main_rest);

        printf("O pedido #%d foi criado!\n", *op_counter);
        (*op_counter)++;
    }
}

void read_status(struct main_data* data, struct semaphores* sems) {
    int num;
    scanf("%d", &num); // guardar id da operacao requisitada em num

    // escrever no log
    write_status(num, configuration->files->log_file);

    if(num > data->max_ops || ((data->results + num)->id == 0 && num != 0) || op_counter == 0){ // verificar se a operacao existe
        printf("Id de pedido fornecido e invalido!\n");
    } else if ((data->results + num)->status == 'I') { 
        //se o pedido ainda nao tiver chegado ao restaurante
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s "
        "ainda não foi recebido no restaurante!\n", 
        num, (data->results + num)->status, (data->results + num)->requesting_client, (data->results + num)->requested_rest,
        ((data->results + num)->requested_dish));
    }  else if ((data->results + num)->status == 'R') { 
        //se o pedido estiver a ser preparado no restaurante
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s "
        "esta a ser tratado pelo restaurante %d!\n", 
        num, (data->results + num)->status, (data->results + num)->requesting_client, (data->results + num)->requested_rest,
        ((data->results + num)->requested_dish), (data->results + num)->receiving_rest);
    }  else if ((data->results + num)->status == 'D') { 
        //se o pedido estiver com o motorista
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s "
        "foi tratado pelo restaurante %d e esta a ser encaminhado pelo motorista %d!\n", 
        num, (data->results + num)->status, (data->results + num)->requesting_client, (data->results + num)->requested_rest,
        ((data->results + num)->requested_dish), (data->results + num)->receiving_rest, (data->results + num)->receiving_driver);
    } else {
        // se for valida imprimir o estado
        printf("Pedido %d com estado %c requisitado pelo cliente %d ao restaurante %d com o prato %s "
        "foi tratado pelo restaurante %d, encaminhado pelo motorista %d, e enviado ao cliente %d!\n", 
        num, (data->results + num)->status, (data->results + num)->requesting_client, (data->results + num)->requested_rest,
        ((data->results + num)->requested_dish), (data->results + num)->receiving_rest, (data->results + num)->receiving_driver,
        (data->results + num)->receiving_client);
    }
}

void stop_execution(struct main_data* data, struct communication_buffers* buffers, struct semaphores* sems) {
    // declarar fim do ciclo
    stop = 1;
    // escrever no log
    write_stop(configuration->files->log_file);
    // colocar data->terminate a 1
    *(data->terminate) = 1;
    // acordar processos
    wakeup_processes(data, sems);
    // esperar que processos terminem
    wait_processes(data);
    // escrever estatisticas finais
    write_statistics(data);
    // fechar ficheiros log e stats
    close_log_stats(configuration->files);
    // libertar semaforos
    destroy_semaphores(sems);
    // libertar zonas de memoria partilhada
    destroy_memory_buffers(data, buffers);
}

void wait_processes(struct main_data* data) {
    // esperar por processos de restaurantes
    for(int i = 0; i < (data->n_restaurants); i++){
         *((data->restaurant_stats) + i) = wait_process(*(data->restaurant_pids + i));
        // escrever o seu valor de retorno nos stats
    }

    // esperar por processos de motoristas
    for(int i = 0; i < (data->n_drivers); i++){
        *((data->driver_stats) + i) = wait_process(*(data->driver_pids + i));
        // escrever o seu valor de retorno nos stats
    }

    // esperar por processos de clientes
    for(int i = 0; i < (data->n_clients); i++){
        *((data->client_stats) + i) = wait_process(*(data->client_pids + i));
        // escrever o seu valor de retorno nos stats
    }
}

void write_statistics(struct main_data* data) {
    // escrever no ficheiro de stats
    write_stats_file(data, configuration->files->stats_file);

    printf("Terminando o MAGNAEATS! Imprimindo estatísticas:\n");

    // imprimir restaurantes
    for(int i = 0; i<data->n_restaurants; i++){
        printf("Restaurante %d preparou %d pedidos\n", i, *(data->restaurant_stats + i));
    }

    // imprimir motoristas
    for(int i = 0; i<data->n_drivers; i++){
        printf("Motorista %d entregou %d pedidos\n", i, *(data->driver_stats + i));
    }
    
    // imprimir clientes
    for(int i = 0; i<data->n_clients; i++){
        printf("Cliente %d recebeu %d pedidos\n", i, *(data->client_stats + i));
    }
}

void destroy_memory_buffers(struct main_data* data, struct communication_buffers* buffers) {
    /* memoria dinamica */
    
    // eliminar memoria dinamica para restaurantes
    destroy_dynamic_memory(data->restaurant_pids);
    destroy_dynamic_memory(data->restaurant_stats);

    // eliminar memoria dinamica para motoristas
    destroy_dynamic_memory(data->driver_pids);
    destroy_dynamic_memory(data->driver_stats);

    // eliminar memoria dinamica para clientes
    destroy_dynamic_memory(data->client_pids);
    destroy_dynamic_memory(data->client_stats);

    // eliminar memoria dinamica para nomes de pedidos
    for(int i = 0; i < op_counter; i++){
        destroy_dynamic_memory((data->results + i)->requested_dish);
    }

    /* memoria partilhada */

    // eliminar memoria para os buffers de communication_buffers
    destroy_shared_memory(STR_SHM_MAIN_REST_BUFFER, (buffers->main_rest)->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_REST_DRIVER_BUFFER, (buffers->rest_driv)->buffer, data->buffers_size);
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_BUFFER, (buffers->driv_cli)->buffer, data->buffers_size);

    // eliminar memoria para os pointers de communication_buffers
    destroy_shared_memory(STR_SHM_MAIN_REST_PTR, (buffers->main_rest)->ptrs, sizeof(int));
    destroy_shared_memory(STR_SHM_REST_DRIVER_PTR, (buffers->rest_driv)->ptrs, sizeof(struct pointers));
    destroy_shared_memory(STR_SHM_DRIVER_CLIENT_PTR, (buffers->driv_cli)->ptrs, sizeof(int));

    // eliminar memoria para data->results
    destroy_shared_memory(STR_SHM_RESULTS, data->results, (data->max_ops)*sizeof(struct operation));

    // eliminar memoria para data->terminate
    destroy_shared_memory(STR_SHM_TERMINATE, data->terminate, sizeof(int));

}

void create_semaphores(struct main_data* data, struct semaphores* sems) {
    // semaforos full
    sems->main_rest->full = semaphore_create(STR_SEM_MAIN_REST_FULL, 0);
    sems->rest_driv->full = semaphore_create(STR_SEM_REST_DRIV_FULL, 0);
    sems->driv_cli->full = semaphore_create(STR_SEM_DRIV_CLI_FULL, 0);

    // semaforos empty
    sems->main_rest->empty = semaphore_create(STR_SEM_MAIN_REST_EMPTY, data->buffers_size);
    sems->rest_driv->empty = semaphore_create(STR_SEM_REST_DRIV_EMPTY, data->buffers_size);
    sems->driv_cli->empty = semaphore_create(STR_SEM_DRIV_CLI_EMPTY, data->buffers_size);

    // semaforos mutex
    sems->main_rest->mutex = semaphore_create(STR_SEM_MAIN_REST_MUTEX, 1);
    sems->rest_driv->mutex = semaphore_create(STR_SEM_REST_DRIV_MUTEX, 1);
    sems->driv_cli->mutex = semaphore_create(STR_SEM_DRIV_CLI_MUTEX, 1);

    // semaforo results
    sems->results_mutex = semaphore_create(STR_SEM_RESULTS_MUTEX, 1);
}

void wakeup_processes(struct main_data* data, struct semaphores* sems) {
    // semaforo main_rest
    for(int i = 0; i < data->n_restaurants; i++){
        produce_end(sems->main_rest);   
    }
    // semaforo rest_driv
    for(int i = 0; i < data->n_drivers; i++){
        produce_end(sems->rest_driv);  
    }

    // semaforo driv_cli
    for(int i = 0; i < data->n_clients; i++){
        produce_end(sems->driv_cli);   
    }
}

void destroy_semaphores(struct semaphores* sems) {
    // semaforos full
    semaphore_destroy(STR_SEM_MAIN_REST_FULL, sems->main_rest->full);
    semaphore_destroy(STR_SEM_REST_DRIV_FULL, sems->rest_driv->full);
    semaphore_destroy(STR_SEM_DRIV_CLI_FULL, sems->driv_cli->full);

    // semaforos empty
    semaphore_destroy(STR_SEM_MAIN_REST_EMPTY, sems->main_rest->empty);
    semaphore_destroy(STR_SEM_REST_DRIV_EMPTY, sems->rest_driv->empty);
    semaphore_destroy(STR_SEM_DRIV_CLI_EMPTY, sems->driv_cli->empty);

    // semaforos mutex
    semaphore_destroy(STR_SEM_MAIN_REST_MUTEX, sems->main_rest->mutex);
    semaphore_destroy(STR_SEM_REST_DRIV_MUTEX, sems->rest_driv->mutex);
    semaphore_destroy(STR_SEM_DRIV_CLI_MUTEX, sems->driv_cli->mutex);

    // semaforo results
    semaphore_destroy(STR_SEM_RESULTS_MUTEX, sems->results_mutex);
}