#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#include<wchar.h>
#include<sys/time.h>
#include "utils.h"
#include "game.h"
#include "menu.h"


int main() {
    clear_screen();
    nonblock_terminal();
    visible_cusor(0);
    
    int m = menu();
    if(m == 1)
        main_game();

    change_color(0, 0); //reset
    visible_cusor(1);
    reset_terminal();
    clear_screen();
}
