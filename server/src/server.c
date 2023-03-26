#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define MESSAGE_SIZE 256
#define MAX_MESSAGE_LEN 40
#define MESSAGE_FORMAT "[%s] %s: %c %.*s"
#define BUFFER_SIZE 20
#define PORT 8080

typedef struct {
    int socket_fd;
    char user_id[BUFFER_SIZE];
    struct sockaddr_in address;
} client_info;

volatile sig_atomic_t server_running = 1;

int client_count = 0;
client_info clients[MAX_CLIENTS];

void *handle_client(void *arg);
void send_to_all(char *message, int sender_socket_fd);
void terminate_server_signal(int sig);
int find_client_index(int socket_fd);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    pthread_t thread_ids[MAX_CLIENTS];
    int thread_count = 0;

    printf("Server started. Waiting for clients...\n");

    while (server_running) {
        if (client_count < MAX_CLIENTS) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                if (server_running)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                else
                {
                    continue;
                }

            }

            client_info *new_client = &clients[client_count++];
            new_client->socket_fd = new_socket;
            new_client->address = address;

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, new_client);
            pthread_detach(thread_id);

            printf("Client connected: %s\n", inet_ntoa(new_client->address.sin_addr));
            
            thread_ids[thread_count++] = thread_id;
        }
    }
    
    // signal all the threads to exit
    for (int i = 0; i < thread_count; i++) 
    {
        pthread_cancel(thread_ids[i]);
    }

    // wait for all the threads to finish
    for (int i = 0; i < thread_count; i++) 
    {
        pthread_join(thread_ids[i], NULL);
    }

    // close all the client sockets
    for (int i = 0; i < client_count; i++) 
    {
        close(clients[i].socket_fd);
    }

    // close the server socket
    close(server_fd);

    return 0;
}

void *handle_client(void *arg) {
    client_info *client = (client_info *)arg;
    int socket_fd = client->socket_fd;
    char buffer[MESSAGE_SIZE];
    ssize_t message_len;

    // Read user ID
    read(socket_fd, client->user_id, sizeof(client->user_id) - 1);
    client->user_id[5] = '\0';

    while ((message_len = read(socket_fd, buffer, MESSAGE_SIZE)) > 0) {
        buffer[message_len] = '\0';

        // Check for exit message
        if (strcmp(buffer, ">>bye<<") == 0) 
        {
            printf("Client %s disconnected: %s\n", client->user_id, inet_ntoa(client->address.sin_addr));
            close(socket_fd);
            client_count--;
            return NULL;
        }

        send_to_all(buffer, socket_fd);
        printf("message sent to all clients\n");
    }

    return NULL;
}


void send_to_all(char *message, int sender_socket_fd) {
    char timestamp[20];
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&now));

    char ip_address[16];
    inet_ntop(AF_INET, &(clients[find_client_index(sender_socket_fd)].address.sin_addr), ip_address, INET_ADDRSTRLEN);

    char direction = '<';
    int client_index = find_client_index(sender_socket_fd);
    if (client_index >= 0) {
        direction = '>';
    }

    int message_len = strlen(message);
    char *formatted_message = malloc(MESSAGE_SIZE);
    int start = 0;

    while (start < message_len) {
        int end = start + MAX_MESSAGE_LEN;
        if (end > message_len) {
            end = message_len;
        }
        int len = end - start;
        char temp[len + 1];
        strncpy(temp, message + start, len);
        temp[len] = '\0';
        snprintf(formatted_message, MESSAGE_SIZE, "%-15s_[%5s]_ %c%c %-*.*s (%s)\n", ip_address, clients[client_index].user_id, direction, direction, MAX_MESSAGE_LEN, MAX_MESSAGE_LEN, temp, timestamp);
    
        for (int i = 0; i < client_count; i++) {
            if (clients[i].socket_fd != sender_socket_fd) {
                send(clients[i].socket_fd, formatted_message, strlen(formatted_message), 0);
            }
        }
        printf("%s", formatted_message);
        start = end;
        usleep(1000); // 1ms delay between sending parts
    }

    free(formatted_message);
}



int find_client_index(int socket_fd) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i].socket_fd == socket_fd) {
            return i;
        }
    }
    return -1;
}


void terminate_server_signal(int sig)
{
    server_running = 0;
}



