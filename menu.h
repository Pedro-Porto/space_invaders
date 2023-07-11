#ifndef MENU_H_
#define MENU_H_
#include "utils.h"

typedef struct {
    int visible;
    coord position;
    coord old_position;
    int side;
} star_struct;

typedef struct {
    char text[50];
    int text_size;
    int selected;
    int state;
} menu_struct;

typedef struct {
    int start_x;
    int end_x;
    int start_y;
    int end_y;
    int active;
} limits_struct;

int menu();

#endif