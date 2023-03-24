#include <ncurses.h>

WINDOW *create_a_window(int height, int width, int startx, int starty);
void input_window(WINDOW *window, char *input, char *user);
void display_window(WINDOW *window, char *input, int theRow, int isItBlank);
void destroy_window(WINDOW *window);
void clear_window(WINDOW *window);