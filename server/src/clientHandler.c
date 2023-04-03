#include "../inc/clientHandler.h"

int client_count = 0;
volatile sig_atomic_t server_running = 1;
client_info clients[MAX_CLIENTS] = {0};
pthread_t thread_ids[MAX_CLIENTS] = {0};

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
    bool first_msg = true;
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
                    
                    if (first_msg)
                    {
                        formatted_message[26] = '&';
                        first_msg = false;
                    }
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

                    if (first_msg)
                    {
                        sender_message[26] = '&';
                        first_msg = false;
                    }
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