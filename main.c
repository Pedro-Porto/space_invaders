#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#include<wchar.h>



typedef struct coord {
    int x;
    int y;
} coord;


coord window_size(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    coord ws = {w.ws_col, w.ws_row};
    return ws;
}


int check_char() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF){
      ungetc(ch, stdin);
      return 1;
    }
    return 0;
}


int x_border_init;
int x_border_fin;
int y_border_init;
int y_border_fin;
int x_size = 150;
int y_size = 45;
int aliens_x = 11;
int aliens_y = 5;
coord ws;





typedef struct shot_struct {
    int visible;
    coord position;
    coord old_position;
    
} shot_struct;



typedef struct ship_struct {
    char sps[10];
    coord position;
    coord old_position;
    shot_struct shots[3];
} ship_struct;

typedef struct alien_struct {
    coord position;
    coord old_position;
    int type;
    int visible;
    int time_count;
} alien_struct;




char alien_img[4][20] = {
    " \\ / |O O|/( )\\",
    " / \\ (O O)/^ ^\\",
    "\\ | /     / | \\",
    "                 "
};





ship_struct ship = {
    .sps = " |^| /(-)\\",
    .old_position = {0, 0},
    .shots = {
        {
            .visible = 0,
            .position = {0, 0},
            .old_position = {0, 0}
        },
        {
            .visible = 0,
            .position = {0, 0},
            .old_position = {0, 0}
        },
        {
            .visible = 0,
            .position = {0, 0},
            .old_position = {0, 0}
        }
    }
};



void print_coord(int x, int y, char c) {
    printf("\033[%d;%dH%c", y + 1, x + 1, c);
}



void print_ship() {
    printf("\033[0;37m"); //branco
    for (int i = 0; i < 10; i++){
        if(ship.old_position.x != ship.position.x || ship.old_position.y != ship.position.y)
            print_coord(ship.old_position.x + (i - ((i / 5) * 5) - 2), ship.old_position.y - !(i / 5), ' ');
        print_coord(ship.position.x + (i - ((i / 5) * 5) - 2), ship.position.y - !(i / 5), ship.sps[i]);
        
    }
    ship.old_position.x = ship.position.x;
    ship.old_position.y = ship.position.y;
}





void print_tab() {
    system("clear");
    printf("\033[0;31m"); //amarelo
    for (int y = 0; y < ws.y; y++)
        for (int x = 0; x < ws.x; x++) {
            if ((x == x_border_init && y >= y_border_init && y <= y_border_fin) || (x == x_border_fin && y >= y_border_init && y <= y_border_fin))
                putchar('#');
            else if ((y == y_border_init && x >= x_border_init && x <= x_border_fin) || (y == y_border_fin && x >= x_border_init && x <= x_border_fin))
                putchar('#');
            else
                putchar(' ');
        }
    fflush(stdout);
}


void print_alien(alien_struct *alien) {
    if(alien->visible){
        if(alien->type == 2)
            printf("\033[0;33m"); //amarelo
        else
            printf("\033[0;32m"); //verde
        if(alien->position.x != alien->old_position.x || alien->position.y != alien->old_position.y)
            for (int i = 0; i < 15; i++)
                print_coord(alien->old_position.x + (i - ((i / 5) * 5) - 2), alien->old_position.y + (i / 5) + 1, ' ');
        for (int i = 0; i < 15; i++)
            print_coord(alien->position.x + (i - ((i / 5) * 5) - 2), alien->position.y + (i / 5) + 1, alien_img[alien->type][i]);
        alien->old_position.x = alien->position.x;
        alien->old_position.y = alien->position.y;
    }
}

void print_shot() {
    for (int i = 0; i < 3; i++) {
        if(ship.shots[i].visible){
            if(ship.shots[i].position.y <= y_border_init){
                ship.shots[i].visible = 0;
                print_coord(ship.shots[i].position.x, ship.shots[i].position.y, ' ');
                continue;
            }
            ship.shots[i].position.y--;
            print_coord(ship.shots[i].old_position.x, ship.shots[i].old_position.y, ' ');
            printf("\033[0;34m"); //azul
            print_coord(ship.shots[i].position.x, ship.shots[i].position.y, '|');
            ship.shots[i].old_position.x = ship.shots[i].position.x;
            ship.shots[i].old_position.y = ship.shots[i].position.y;
        }
    }
    
}



void alien_constructor(alien_struct *aliens) {
    int mid = (x_border_fin - x_border_init) / 2 + x_border_init;
    for (int y = 0; y < aliens_y; y++)
        for (int x = 0; x < aliens_x; x++) {
            aliens->position.x = (mid - 40) + (x * 8);
            aliens->type = (y % 2);
            aliens->position.y = y_border_init + 2 + (y * 5);
            aliens->visible = 1;
            aliens->time_count = 0;
            aliens->old_position.x = aliens->position.x;
            aliens->old_position.y = aliens->position.y;
            print_alien(aliens);
            aliens++;
        }
}



void sleep_ms(int ms) {
    double start_t, start_t2, end_t;
    start_t = ((double)clock() / CLOCKS_PER_SEC);
    while(end_t - start_t < (double)(ms / 1000)){
        end_t = ((double)clock() / CLOCKS_PER_SEC);
    }
}



void main_game() {
    double start_t, start_t2, end_t;
    int ch;
    int alien_direction = 0; // 0 = direita; 1 = esquerda
    ws = window_size();
    x_border_init = (ws.x - x_size) / 2;
    x_border_fin = x_border_init + x_size;
    y_border_init = (ws.y - y_size) / 2;
    y_border_fin = y_border_init + y_size;

    print_tab();

    usleep(10000);

    ship.position.x = (x_border_fin - x_border_init) / 2 + x_border_init;
    ship.position.y = y_border_fin - 3;

    print_ship();

    alien_struct aliens[aliens_y*aliens_x];
    alien_constructor(aliens);

    fflush(stdout);
    start_t = ((double)clock() / CLOCKS_PER_SEC);
    start_t2 = ((double)clock() / CLOCKS_PER_SEC);
    while(1) {
        ch = getchar();
        if(ch != EOF){
            if(ch == 'a') {
                if(ship.position.x - 5 > x_border_init){
                    ship.position.x-=5;
                    print_ship();
                }
            }
            if(ch == 'd') {
                if(ship.position.x + 5 < x_border_fin){
                    ship.position.x+=5;
                    print_ship();
                }
            }
            if(ch == 'w') {
                if(ship.position.y + 2 > y_border_init){
                    ship.position.y-=2;
                    print_ship();
                }
            }
            if(ch == 's') {
                if(ship.position.y - 2 < y_border_fin){
                    ship.position.y+=2;
                    print_ship();
                }
            }
            if(ch == 'x')
                break;
            if(ch == ' '){
                for (int i = 0; i < 3; i++) {
                    if(!ship.shots[i].visible){
                        ship.shots[i].visible = 1;
                        ship.shots[i].position.x = ship.position.x;
                        ship.shots[i].position.y = ship.position.y - 2;
                        break;
                    }
                }
                
                
            }
        }
        end_t = ((double)clock() / CLOCKS_PER_SEC);
        if(end_t - start_t >= 0.03) {
            start_t = ((double)clock() / CLOCKS_PER_SEC);
            print_shot();
            for (int i = 0; i < aliens_y*aliens_x; i++) {
                if(aliens[i].type == 2) {
                    aliens[i].time_count++;
                    if(aliens[i].time_count == 4){
                        aliens[i].time_count = 0;
                        aliens[i].type = 3;
                        print_alien(&aliens[i]);
                        aliens[i].visible = 0;
                    }
                    
                }

                for (int j = 0; j < 3; j++) {
                    if(aliens[i].type != 2 && aliens[i].type != 3 && ship.shots[j].visible && ship.shots[j].position.x >= aliens[i].position.x - 2 && ship.shots[j].position.x <= aliens[i].position.x + 2 && ship.shots[j].position.y >= aliens[i].position.y - 1 && ship.shots[j].position.y <= aliens[i].position.y + 1){
                        ship.shots[j].visible = 0;
                        aliens[i].type = 2;
                        print_alien(&aliens[i]);
                    }
                }

            }
        }
        if(end_t - start_t2 >= 1){
            start_t2 = ((double)clock() / CLOCKS_PER_SEC);
            int al_max_coord = alien_direction*1000;
            for (int i = 0; i < aliens_y*aliens_x; i++) {
                if(!alien_direction && aliens[i].position.x > al_max_coord && aliens[i].visible)
                    al_max_coord = aliens[i].position.x;
                else if(aliens[i].position.x < al_max_coord && aliens[i].visible)
                    al_max_coord = aliens[i].position.x;
            }
            
            if((!alien_direction && al_max_coord + 5 >= x_border_fin) || (alien_direction && al_max_coord - 5 <= x_border_init)){
                alien_direction = !alien_direction;
                for (int i = aliens_y*aliens_x - 1; i >= 0; i--) {
                    aliens[i].position.y += 3;
                    print_alien(&aliens[i]);
                }
            }
            else {
                if(alien_direction){
                    for (int i = aliens_y - 1; i >= 0; i--) {
                        for (int j = 0; j < aliens_x; j++) {
                            aliens[i*aliens_x + j].position.x -= 5;
                            print_alien(&aliens[i*aliens_x + j]);
                        }
                    }
                }
                else{
                    for (int i = aliens_y*aliens_x - 1; i >= 0; i--) {
                        aliens[i].position.x += 5;
                        print_alien(&aliens[i]);
                    }
                }
            }
        }
        sleep_ms(1);
    }
}


int main() {
    system("clear");
    printf("\e[?25l"); //desativar cursor
    struct termios oldt, newt;
    
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    main_game();

    printf("\e[?25h"); //ativar cursor

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    system("clear");
}
