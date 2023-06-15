#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#include<wchar.h>


struct winsize w;

typedef struct coord {
    int x;
    int y;
} coord;


void print_coord(int x, int y, char *c) {
    printf("\033[%d;%dH%s", y + 1, x + 1, c);
}