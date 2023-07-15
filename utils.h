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

int rand_range(int, int);

double get_time();
// coor
void print_coord(int, int, char);

void print_tab();

void change_color(int, int);

void visible_cusor(int);

void nonblock_terminal();

void reset_terminal();

void clear_screen();

void msleep(int);

int intlen(int);

void prints_coord(int, int, char*);

void printi_coord(int, int, int);

#endif