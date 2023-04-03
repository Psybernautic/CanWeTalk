/*
FILE                : winControl.h
PROJECT             : Can We Talk?
PROGRAMMER          : Sebastian Posada, Angel Aviles, Jonathon Gregoric
FIRST VERSION       : 2023-03-20
DESCRIPTION         : This file contains the prototypes of the functions used for the
					displays creation and management by the use of ncurses
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ncurses.h>

WINDOW *create_a_window(int height, int width, int startx, int starty);
void input_window(WINDOW *window, char *input, char *user);
void display_window(WINDOW *window, char *input, int theRow, int isItBlank);
void destroy_window(WINDOW *window);
void clear_window(WINDOW *window);