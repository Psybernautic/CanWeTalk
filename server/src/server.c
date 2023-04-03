/*
FILE                : server.c
PROJECT             : Can We Talk?
PROGRAMMER          : Sebastian Posada, Angel Aviles, Jonathon Gregoric
FIRST VERSION       : 2023-03-20
DESCRIPTION         : This file contains the main entry point
                    of the server side application of the chat server.
*/

#include "../inc/clientHandler.h"

int main()
{
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

    //printf("Server started. Waiting for clients...\n");

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

            //printf("Client connected: %s\n", inet_ntoa(new_client->address.sin_addr));
            
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
