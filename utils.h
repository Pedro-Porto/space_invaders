#ifndef UTILS_H_
#define UTILS_H_

typedef struct {
    int x;
    int y;
} coord;

extern coord ws;
extern int x_border_init;
extern int x_border_fin;
extern int y_border_init;
extern int y_border_fin;
extern int x_size;
extern int y_size;
extern struct termios oldt;
extern int oldf;

coord window_size();

int rand_range(int minimum_number, int max_number);

double get_time();

void print_coord(int x, int y, char c);

void print_tab();

void change_color(int color, int bg);

void visible_cusor(int state);

void nonblock_terminal();

void reset_terminal();

void clear_screen();

void msleep(int millis);

int intlen(int i);

void prints_coord(int x, int y, char *c);

void printi_coord(int x, int y, int i);

#endif