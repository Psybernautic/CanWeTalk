#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_

#define MAX_CLIENTS 10
#define MESSAGE_SIZE 79
#define INCOMING_MSG_SIZE 80
#define MAX_MESSAGE_LEN 40
#define BUFFER_SIZE 20
#define USER_NAME_SIZE 6
#define TIMESTAMP_SIZE 9
#define PORT 8080

extern int client_count;
extern volatile sig_atomic_t server_running;

typedef struct {
    int socket_fd;
    char user_id[USER_NAME_SIZE];
    struct sockaddr_in address;
} client_info;

extern client_info clients[MAX_CLIENTS];
extern pthread_t thread_ids[MAX_CLIENTS];

void *handle_client(void *arg);
void send_to_all(char *message, int sender_socket_fd);
void terminate_server_signal(int sig);
int find_client_index(int socket_fd);

#endif