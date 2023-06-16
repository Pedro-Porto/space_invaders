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
int x_size = 102;
int y_size = 45;
int aliens_x = 10;
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
    int type;
    int visible;
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
    
    for (int i = 0; i < 10; i++){
        if(ship.old_position.x + (i - ((i / 5) * 5) - 2) != ship.position.x + (i - ((i / 5) * 5) - 2))
            print_coord(ship.old_position.x + (i - ((i / 5) * 5) - 2), ship.old_position.y - !(i / 5), ' ');
        printf("\033[0;37m"); //branco
        print_coord(ship.position.x + (i - ((i / 5) * 5) - 2), ship.position.y - !(i / 5), ship.sps[i]);
        
    }
    ship.old_position.x = ship.position.x;
    ship.old_position.y = ship.position.y;
}





void print_tab() {
    system("clear");
    // 90 
    
    
    
    printf("%d %d \n", ws.x, ws.y);
    for (int y = 0; y < ws.y; y++)
        for (int x = 0; x < ws.x; x++) {
            printf("\033[0;32m"); //amarelo
            if ((x == x_border_init && y >= y_border_init && y <= y_border_fin) || (x == x_border_fin && y >= y_border_init && y <= y_border_fin))
                putchar('#');
            else if ((y == y_border_init && x >= x_border_init && x <= x_border_fin) || (y == y_border_fin && x >= x_border_init && x <= x_border_fin))
                putchar('#');
            else
                putchar(' ');
        }
        
    


}


void print_alien(alien_struct alien) {
    for (int i = 0; i < 15; i++)
        print_coord(alien.position.x + (i - ((i / 5) * 5) - 2), alien.position.y + (i / 5) + 1, alien_img[alien.type][i]);
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
    int mid_l;
    int mid_r;
    if(!(aliens_x % 2)){
        mid_l = ((x_border_fin - x_border_init) / 2 + x_border_init) - 4;
        mid_r = ((x_border_fin - x_border_init) / 2 + x_border_init) + 4;
    }
    else {
        mid_l = (x_border_fin - x_border_init) / 2 + x_border_init;
        mid_r = (x_border_fin - x_border_init) / 2 + x_border_init;
    }

    for (int y = 0; y < aliens_y; y++)
        for (int x = 0; x < aliens_x; x++) {
            if(x < aliens_x / 2) 
                aliens->position.x = mid_l - (((aliens_x / 2) - x - 1) * 8);
            else 
                aliens->position.x = mid_r + (x - (aliens_x / 2)) * 8;

            aliens->type = (y % 2);
            aliens->position.y = y_border_init + 2 + (y * 5);
            aliens->visible = 1;
            print_alien(*aliens);
            aliens++;

        }
}



int main() {
    double start_t, end_t;
    ws = window_size();
    x_border_init = (ws.x - x_size) / 2;
    x_border_fin = x_border_init + x_size;
    y_border_init = (ws.y - y_size) / 2;
    y_border_fin = y_border_init + y_size;



    print_tab();

    ship.position.x = (x_border_fin - x_border_init) / 2 + x_border_init;
    ship.position.y = y_border_fin - 3;

    print_ship();

    alien_struct aliens[aliens_y*aliens_x];
    alien_constructor(aliens);







    printf("\e[?25l"); //desativar cursor
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    start_t = ((double)clock() / CLOCKS_PER_SEC)*1000;
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
        end_t = ((double)clock() / CLOCKS_PER_SEC)*1000;
        if(end_t - start_t >= 0.1) {
            start_t = ((double)clock() / CLOCKS_PER_SEC)*1000;
            print_shot();
            for (int i = 0; i < aliens_y*aliens_x; i++) {
                for (int j = 0; j < 3; j++) {
                    if(aliens[i].visible && ship.shots[j].visible && ship.shots[j].position.x >= aliens[i].position.x - 2 && ship.shots[j].position.x <= aliens[i].position.x + 2 && ship.shots[j].position.y >= aliens[i].position.y - 1 && ship.shots[j].position.y <= aliens[i].position.y + 1){
                        ship.shots[j].visible = 0;
                        aliens[i].visible = 0;
                        aliens[i].type = 3;
                        print_alien(aliens[i]);
                    }
            }
            }
            
        }
        usleep(10000);

    }
    
    printf("\e[?25h"); //ativar cursor

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    system("clear");
    

}