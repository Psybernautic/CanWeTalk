/*
FILE                : client.h
PROJECT             : Can We Talk?
PROGRAMMER          : Sebastian Posada, Angel Aviles, Jonathon Gregoric
FIRST VERSION       : 2023-03-20
DESCRIPTION         : This file contains the functions used
                    in the client source code for receiving messages
                    from different clients across a chat server
*/

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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#define MESSAGE_SIZE 79
#define MAX_OUT_MSG_SIZE 80
#define BUFFER_SIZE 256
#define PREDEF_WIDTH 90
#define PORT 8080
#define MAX_ROW 10

typedef struct args {
    int sock;
    WINDOW *window_show;
} ThreadArgs;

void *receive_messages(void *arg);


//
// FUNCTION     : receive_messages
// DESCRIPTION  :
//  This function receives messages from a socket and displays them in a window.
//  The function takes a pointer to a ThreadArgs struct that contains the socket and window to use.
//  If the message received is a continuation of a previous message, it is displayed in a new row.
//  The function has a maximum row limit and clears the window when the limit is reached.
//  The buffer is cleared and there is a short delay before continuing to receive messages.
//
// PARAMETERS   :
//  void *arg    : a pointer to a ThreadArgs struct that contains the socket and window to use
//
// RETURNS      :
//  void *       : NULL when the function completes successfully
void *receive_messages(void *arg)
{
    // Extract socket and window from the argument struct
    int sock = ((ThreadArgs *)arg)->sock;

    // Initialize buffer to store incoming messages
    char buffer[MESSAGE_SIZE + 1] = "";

    // Keep track of the current row in the window
    int row = 0;

    // Keep track of the length of the received message
    ssize_t message_len;

    // Infinite loop to continuously receive messages
    while (1)
    {
        // Receive a message from the socket
        message_len = recv(sock, buffer, MESSAGE_SIZE, 0);
        // If the message length is zero or negative, break out of the loop
        if (message_len <= 0)
        {
            break;
        }
        else
        {
            // If the current row has reached the maximum limit, display the message in a new window
            if (row >= MAX_ROW)
            {
                // Check if this is a new message or a continuation of a previous message
                if (buffer[67] == '&' && buffer[26] != '&')
                {
                    // Replace '&' with a space and display the message in a new row
                    buffer[67] = ' ';
                    display_window(((ThreadArgs *)arg)->window_show, buffer, row, 0);
                    fflush(stdout);
                    ++row;
                }
                else
                {
                    // If this is a continuation of a previous message, clear the window and start a new row
                    row = 0;
                    buffer[67] = ' ';
                    buffer[26] = ' ';
                    display_window(((ThreadArgs *)arg)->window_show, buffer, row, 1);
                    fflush(stdout);
                    ++row;
                }
            }
            else
            {
                // If the message fits within the current row, display it in the same row
                if (buffer[67] == '&')
                {
                    // Check if this is a partial message
                    buffer[67] = ' ';
                    if (buffer[26] == '&')
                    {
                        // Replace '&' with a space
                        buffer[26] = ' ';
                    }
                }
                
                // Display the message in the window and increment the row count
                display_window(((ThreadArgs *)arg)->window_show, buffer, row, 0);
                fflush(stdout);
                ++row;
            }

            // Clear the buffer and wait for a short period before continuing
            memset(buffer, 0, sizeof(buffer));
            usleep(2000);
        }
    }

    // Return NULL when the function is finished
    return NULL;
}


#endif