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

#define MAX_CLIENTS 10
#define MESSAGE_SIZE 79
#define INCOMING_MSG_SIZE 80
#define MAX_MESSAGE_LEN 40
#define BUFFER_SIZE 20
#define USER_NAME_SIZE 6
#define TIMESTAMP_SIZE 9
#define PORT 8080

typedef struct {
    int socket_fd;
    char user_id[USER_NAME_SIZE];
    struct sockaddr_in address;
} client_info;

volatile sig_atomic_t server_running = 1;

int client_count = 0;
client_info clients[MAX_CLIENTS] = {0};
pthread_t thread_ids[MAX_CLIENTS] = {0};

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
            new_client->user_id[0] = '\0';

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
    char bufferName[BUFFER_SIZE] = "";
    char buffer[INCOMING_MSG_SIZE + 1] = "";
    ssize_t message_len;

    // Read user ID
    read(socket_fd, bufferName, sizeof(bufferName));
    strncpy(client->user_id, bufferName, USER_NAME_SIZE - 1);
    client->user_id[5] = '\0';

    while ((message_len = read(socket_fd, buffer, INCOMING_MSG_SIZE)) > 0) {
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
        usleep(2000);
        printf("message sent to all clients\n");
    }

    return NULL;
}


void send_to_all(char *message, int sender_socket_fd) {
    char timestamp[TIMESTAMP_SIZE] = "";
    time_t now = time(NULL);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localtime(&now));

    char ip_address[16] = "";
    inet_ntop(AF_INET, &(clients[find_client_index(sender_socket_fd)].address.sin_addr), ip_address, INET_ADDRSTRLEN);

    char direction_send = '>';
    char direction_receive = '<';
    bool sectioned = false;
    int client_index = find_client_index(sender_socket_fd);
    if (client_index >= 0) {
        direction_send = '<';
        direction_receive = '>';
    }

    int message_len = strlen(message);

    // Checking if this message will be divided
    // into two messages
    if (message_len > MAX_MESSAGE_LEN)
    {
        sectioned = true;
    }
    
    int start = 0;

    while (start < message_len) {
        
        int end = start + MAX_MESSAGE_LEN;
        if (end > message_len)
        {
            end = message_len;
        }

        int len = end - start;
        
        // Checking if we are in a space character just where we are cutting
        // the message
        if (end != message_len && message[len -1] != ' ' 
            && message[len] != ' ')
        {
            int index = len - 1;
            while (index >= 0)
            {
                if (message[index] == ' ')
                {
                    len = index + 1;
                    end = len + start;
                    break;
                }
                --index;
            }
        }
        
        char temp[len];

        strncpy(temp, message + start, len);
        temp[len] = '\0';

        for (int i = 0; i < client_count; i++)
        {    
            if (clients[i].socket_fd != sender_socket_fd)
            {
                char *formatted_message = (char *)malloc(MESSAGE_SIZE * sizeof(char));
                snprintf
                (
                    formatted_message, MESSAGE_SIZE, "%-15s [%-5s] %c%c %-*.*s (%s)",
                    ip_address, clients[client_index].user_id, direction_send, direction_send,
                    MAX_MESSAGE_LEN, MAX_MESSAGE_LEN, temp, timestamp
                );

                // Lettimng the client know this is a partial message
                if (sectioned)
                {
                    formatted_message[67] = '&';
                }

                // Sending the message to client
                send(clients[i].socket_fd, formatted_message, strlen(formatted_message), 0);
                // Debug print
                printf("[%-*.*s]\n", MESSAGE_SIZE-1, MESSAGE_SIZE-1, formatted_message);
                // End debug print
                free(formatted_message);
            } 
            else
            {
                char *sender_message = (char *)malloc(MESSAGE_SIZE * sizeof(char));
                snprintf
                (
                    sender_message, MESSAGE_SIZE, "%-15s [%-5s] %c%c %-*.*s (%s)",
                    ip_address, clients[client_index].user_id, direction_receive, direction_receive,
                    MAX_MESSAGE_LEN, MAX_MESSAGE_LEN, temp, timestamp
                );

                // Letting the client know this is a partial message
                if (sectioned)
                {
                    sender_message[67] = '&';
                }

                // Sending the message to client
                send(sender_socket_fd, sender_message, strlen(sender_message), 0);
                // Debug print
                printf("[%-*.*s]\n", MESSAGE_SIZE-1, MESSAGE_SIZE-1, sender_message);
                // End debug print
                free(sender_message);
            }
        }
        start = end;
        usleep(1000); // 1ms delay between sending parts
    }
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



