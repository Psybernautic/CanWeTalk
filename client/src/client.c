/*
FILE                : client.c
PROJECT             : Can We Talk?
PROGRAMMER          : Sebastian Posada, Angel Aviles, Jonathon Gregoric
FIRST VERSION       : 2023-03-20
DESCRIPTION         : This file contains the main entry point
                    of the client side application of the chat server.
*/


#include "../inc/winControl.h"
#include "../inc/client.h"



int main(int argc, char *argv[]) {

    int sock = 0;
    struct sockaddr_in serv_addr;
    ThreadArgs *theArg = (ThreadArgs *)malloc(sizeof(ThreadArgs));
    char *user_id = NULL;
    char *server_name = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-user", 5) == 0) {
            user_id = argv[i] + 5;
        } else if (strncmp(argv[i], "-server", 7) == 0) {
            server_name = argv[i] + 7;
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

    /* create the window where all mesages will be displayed */
    msgs_window = create_a_window(msg_height, msg_width, msg_starty, msg_startx);
    scrollok(msgs_window, TRUE);

    /* create the window where the message to send will be entered */
    chat_window = create_a_window(chat_height, chat_width, chat_starty, chat_startx);
    scrollok(chat_window, TRUE);

    theArg->sock = sock;
    theArg->window_show = msgs_window;

    // Start message receiving thread
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void *)theArg);
    pthread_detach(receive_thread);

    char message[MAX_OUT_MSG_SIZE + 1] = "";
 

    while (1) 
    {
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

            break;
        }

        send(sock, message, strlen(message), 0);
        usleep(2000);
        // clear input buffer
        memset(message, 0, sizeof(message));
    }
    
    destroy_window(chat_window);
    destroy_window(msgs_window);
    endwin();
    free(theArg);
    close(sock);
    return 0;
    
}