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

#include "../inc/winControl.h"

#define MESSAGE_SIZE 2000
#define BUFFER_SIZE 256
#define PORT 8080
#define MAX_ROW 10

typedef struct args {
    int sock;
    WINDOW *window_show;
} ThreadArgs;

void *receive_messages(void *arg);


int main(int argc, char *argv[]) {

    int sock = 0;
    struct sockaddr_in serv_addr;
    ThreadArgs *theArg = (ThreadArgs *)malloc(sizeof(ThreadArgs));
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

    /* Declaring the window */
    WINDOW *chat_window;
    WINDOW *msgs_window;

    /* Starting the ncurses mode */
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();

    int shouldBlank = 0;

    int chat_height = 5;
    int chat_width  = COLS - 2;
    int chat_startx = 1;
    int chat_starty = LINES - chat_height;
        
    int msg_height = LINES - chat_height - 1;
    int msg_width  = COLS;
    int msg_startx = 0;
    int msg_starty = 0;

    /* create the input window */
    msgs_window = create_a_window(msg_height, msg_width, msg_starty, msg_startx);
    scrollok(msgs_window, TRUE);

    /* create the output window */
    chat_window = create_a_window(chat_height, chat_width, chat_starty, chat_startx);
    scrollok(chat_window, TRUE);

    theArg->sock = sock;
    theArg->window_show = msgs_window;

    // Start message receiving thread
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void *)theArg);
    pthread_detach(receive_thread);

    char message[MESSAGE_SIZE + 1] = "";
 

    while (1) 
    {
        /*
        printf("%s> ", user_id);
        fflush(stdout);
        fgets(message, MESSAGE_SIZE, stdin);
        */
        // Replace new line character with null terminator
        input_window(chat_window, message, user_id);

        int len = strlen(message);
        if (len > 0 && message[len - 1] == '\n')
        {
            message[len - 1] = '\0';
        }

        // Check for exit message
        if (strcmp(message, ">>bye<<") == 0) 
        {
            send(sock,message,strlen(message),0);
            destroy_window(chat_window);
            destroy_window(msgs_window);
            endwin();
            break;
        }

        send(sock, message, strlen(message), 0);
        usleep(2000);
        // clear input buffer
        memset(message, 0, sizeof(message));
    }
    
    free(theArg);
    close(sock);
    return 0;
    
}



void *receive_messages(void *arg)
{
    int sock = ((ThreadArgs *)arg)->sock;
    char buffer[BUFFER_SIZE +1];
    int row = 0;
    ssize_t message_len;


    while ((message_len = recv(sock, buffer, MESSAGE_SIZE, 0)) > 0)
    {
        buffer[message_len] = '\0';

        // Display received message
        display_window(((ThreadArgs *)arg)->window_show, buffer, row, 0);
        fflush(stdout);
        ++row;

        if (row >= MAX_ROW) {
            row = 0;
            wclear(((ThreadArgs *)arg)->window_show);
        }
    }

    return NULL;
}
