#include<stdio.h>
#include<stdlib.h>
#include "utils.h"
#include "game.h"
#include "menu.h"


int main() {
    clear_screen();
    nonblock_terminal();
    visible_cusor(0);
    // while(1){
    //     char ch = getchar();
    //     if(ch != EOF){
    //         printf("%d", ch);
    //     }
    // }
    
    int m = menu();
    if(m == 1)
        main_game();

    change_color(0, 0); //reset
    visible_cusor(1);
    reset_terminal();
    clear_screen();
}
