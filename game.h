#ifndef GAME_H_
#define GAME_H_
#include "utils.h"

typedef struct {
    int visible;
    coord position;
    coord old_position;
    
} shot_struct;

typedef struct {
    char sps[10];
    coord position;
    coord old_position;
    shot_struct shots[3];
} ship_struct;

typedef struct {
    coord position;
    coord old_position;
    int type;
    int visible;
    int time_count;
} alien_struct;

void main_game();

#endif