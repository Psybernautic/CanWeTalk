#include <arpa/inet.h>
#include <errno.h>
#include <ncurses.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MESSAGE_SIZE 80
#define BUFFER_SIZE 256
#define PORT 8080

void *receive_messages(void *arg);


int main(int argc, char *argv[]) {

    int sock = 0;
    struct sockaddr_in serv_addr;
    char *user_id = NULL;
    char *server_name = NULL;
    

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-user") == 0 && i + 1 < argc) {
            user_id = argv[++i];
        } else if (strcmp(argv[i], "-server") == 0 && i + 1 < argc) {
            server_name = argv[++i];
        }
    }
    

    if (user_id == NULL || server_name == NULL) {
        fprintf(stderr, "Usage: chat-client -user<userID> -server<server_name>\n");
        return 1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert server name or IP address to binary form
    if (inet_pton(AF_INET, server_name, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    // Send user ID
    send(sock, user_id, strlen(user_id), 0);

    

    // Start message receiving thread
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, &sock);
    pthread_detach(receive_thread);

    char message[MESSAGE_SIZE + 1];
 

    while (1) 
    {
        printf("%s> ", user_id);
        fflush(stdout);
        fgets(message, MESSAGE_SIZE, stdin);
        
        // Replace new line character with null terminator
        int len = strlen(message);
        if (len > 0 && message[len - 1] == '\n')
         {
            message[len - 1] = '\0';
            
         }

        // Check for exit message
        if (strcmp(message, ">>bye<<") == 0) {
            send(sock,message,strlen(message),0);
            break;
        }

        send(sock, message, strlen(message), 0);

        // clear input buffer
        memset(message, 0, sizeof(message));
    }

    close(sock);
    return 0;
    
}



void *receive_messages(void *arg) {
    int sock = *(int *)arg;
    char buffer[BUFFER_SIZE +1];
    ssize_t message_len;



    while ((message_len = recv(sock, buffer, MESSAGE_SIZE, 0)) > 0) {
        buffer[message_len] = '\0';

        // Display received message
        printf("%s", buffer);
        fflush(stdout);
    }

    return NULL;
}
