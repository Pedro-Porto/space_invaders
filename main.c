#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include<fcntl.h>
#include<wchar.h>
#include <sys/time.h>


typedef struct {
    int x;
    int y;
} coord;


coord window_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    coord ws = {w.ws_col, w.ws_row};
    return ws;
}


unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

int rand_range(int minimum_number, int max_number){
    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}


double get_time() {
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    double t = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    return t/1000;
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





typedef struct {
    int visible;
    coord position;
    coord old_position;
    
} shot_struct;


typedef struct {
    int visible;
    coord position;
    coord old_position;
    int side;
} star_struct;


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
    printf("\033[0;31m"); //vermelho
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
    int mid = (x_border_fin + x_border_init) / 2;
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

    usleep(1000);

    ship.position.x = (x_border_fin - x_border_init) / 2 + x_border_init;
    ship.position.y = y_border_fin - 3;

    print_ship();

    alien_struct aliens[aliens_y*aliens_x];
    alien_constructor(aliens);

    fflush(stdout);
    start_t = get_time();
    start_t2 = get_time();
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
        end_t = get_time();
        if(end_t - start_t >= 0.03) {
            start_t = get_time();
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
                        ship.shots[j].visible = 0; //imprimir o tiro de novo
                        aliens[i].type = 2;
                        print_alien(&aliens[i]);
                    }
                }

            }
        }
        if(end_t - start_t2 >= 1){
            start_t2 = get_time();
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
                    aliens[i].position.y += 2;
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
    }
}


void print_star(star_struct *star, limits_struct *limits) {
    int print_new = 1;
    int print_old = 1;
    for (int i = 0; i < 3; i++) {
        if(limits->active){
            if(star->position.x > limits->start_x - 1 && star->position.x < limits->end_x + 1 && star->position.y > limits->start_y - 1 && star->position.y < limits->end_y + 1)
                print_new = 0;
            if(star->old_position.x > limits->start_x - 1 && star->old_position.x < limits->end_x + 1 && star->old_position.y > limits->start_y - 1 && star->old_position.y < limits->end_y + 1)
                print_old = 0;
        }
        limits++;
    }
    if(print_old)
        print_coord(star->old_position.x, star->old_position.y, ' ');
    if(print_new) {
        printf("\033[0;37m"); //branco
        print_coord(star->position.x, star->position.y, star->side == 1 ? '\\' : '/');
    }
    star->old_position = star->position;
}

void change_menu_selection(menu_struct *menu_itens, limits_struct limits, int selection){
    for (int i = 0; i < 4; i++) {
        int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens->text_size / 2);
        int c_y = limits.start_y + (i * 3 + 3);
        if(i != selection && menu_itens->selected){
            menu_itens->selected = 0;
            menu_itens->state = 1;
            printf("\033[0m"); //reset
            printf("\033[0;37m"); //branco
            for (int j = 0; j < menu_itens->text_size; j++) {
                print_coord(c_x + j, c_y, menu_itens->text[j]);
            }
            print_coord(c_x - 1, c_y, ' ');
            print_coord(c_x + menu_itens->text_size, c_y, ' ');
        }
        if(!menu_itens->selected && i == selection) {
            menu_itens->selected = 1;
            
            printf("\033[0;37m\033[41m"); //branco / fundo vermelho
            for (int j = 0; j < menu_itens->text_size; j++) {
                print_coord(c_x + j, c_y, menu_itens->text[j]);
            }
            printf("\033[0m"); //reset
            printf("\033[0;37m"); //branco
            print_coord(c_x - 1, c_y, '<');
            print_coord(c_x + menu_itens->text_size, c_y, '>');
        }
        menu_itens++;
    }
}

void switch_selection_state(menu_struct *menu_itens, limits_struct limits, int selection) {
    if(menu_itens[selection].state){
        printf("\033[0m"); //reset
        printf("\033[0;37m"); //branco
    }
    else {
        printf("\033[0;37m\033[41m"); //branco / fundo vermelho
    }
    menu_itens[selection].state = !menu_itens[selection].state;
    int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens[selection].text_size / 2);
    int c_y = limits.start_y + (selection * 3 + 3);
    for (int i = 0; i < menu_itens[selection].text_size; i++) {
        print_coord(c_x + i, c_y, menu_itens[selection].text[i]);
    }
    printf("\033[0m"); //reset
}

int menu(){
    int ch;
    int stars_count = 150;
    int menu_selection = 0;
    int menu_state = 0;
    star_struct stars[stars_count];
    limits_struct limits[3] = {
        {
            .active = 0
        },
        {
            .active = 0
        },
        {
            .active = 0
        }
    };
    
    menu_struct menu_itens[4] = {
        {
            .text = "Start",
            .text_size = 5,
            .selected = 0,
            .state = 1
        },
        {
            .text = "Highscore leaderboard",
            .text_size = 21,
            .selected = 0,
            .state = 1
        },
        {
            .text = "Settings",
            .text_size = 8,
            .selected = 0,
            .state = 1
        },
        {
            .text = "Quit",
            .text_size = 4,
            .selected = 0,
            .state = 1
        }
    };
    for (int i = 0; i < stars_count; i++){
        stars[i].visible = 0;
    }
    
    ws = window_size();
    x_border_init = (ws.x - x_size) / 2;
    x_border_fin = x_border_init + x_size;
    y_border_init = (ws.y - y_size) / 2;
    y_border_fin = y_border_init + y_size;
    print_tab();

    char title[450] = "       ____                                    / ___| _ __   __ _  ___ ___              \\___ \\| '_ \\ / _` |/ __/ _ \\              ___) | |_) | (_| | (_|  __/             |____/| .__/ \\__,_|\\___\\___|        ___        |_|          _               |_ _|_ ____   ____ _  __| | ___ _ __ ___  | || '_ \\ \\ / / _` |/ _` |/ _ \\ '__/ __| | || | | \\ V / (_| | (_| |  __/ |  \\__ \\|___|_| |_|\\_/ \\__,_|\\__,_|\\___|_|  |___/";
    // 10 x 41
    double start_t, start_t2, end_t;
    limits[0].start_x = (x_border_fin + x_border_init) / 2 - 20;
    limits[0].start_y = y_border_init + 5;
    limits[0].end_x = limits[0].start_x + 41;
    limits[0].end_y = limits[0].start_y + 10;
    limits[0].active = 1;
    printf("\033[0;36m"); //ciano
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 41; j++) {
            print_coord(limits[0].start_x + j, limits[0].start_y + i, title[i*41 + j]);
        }
    }
    printf("\033[0;31m"); //vermelho
    limits[1].start_x = (x_border_fin + x_border_init) / 2 - 15;
    limits[1].end_x = limits[1].start_x + 30;
    limits[1].end_y = y_border_fin - 4;
    limits[1].start_y = limits[1].end_y - 15;
    limits[1].active = 1;
    for (int y = y_border_init; y < y_border_fin; y++)
        for (int x = x_border_init; x < x_border_fin; x++) {
            if ((x == limits[1].start_x && y >= limits[1].start_y + 1 && y <= limits[1].end_y) || (x == limits[1].end_x && y >= limits[1].start_y + 1 && y <= limits[1].end_y))
                print_coord(x, y, '|');
            else if ((y == limits[1].start_y && x >= limits[1].start_x + 1 && x <= limits[1].end_x - 1) || (y == limits[1].end_y && x >= limits[1].start_x && x <= limits[1].end_x))
                print_coord(x, y, '_');
        }
    printf("\033[0;37m"); //branco
    for (int i = 0; i < 4; i++){
        int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens[i].text_size / 2);
        int c_y = limits[1].start_y + (i * 3 + 3);
        for (int j = 0; j < menu_itens[i].text_size; j++) {
            print_coord(c_x + j, c_y, menu_itens[i].text[j]);
        }
        
    }
    limits[2].start_x = (x_border_fin + x_border_init) / 2 - 40;
    limits[2].start_y = (y_border_fin + y_border_init) / 2 - 20;
    limits[2].end_x = limits[2].start_x + 80;
    limits[2].end_y = limits[2].start_y + 40;
    
    

    change_menu_selection(menu_itens, limits[1], menu_selection);
    fflush(stdout);
    start_t = get_time();
    start_t2 = start_t;
    while(1){
        ch = getchar();
        if(ch != EOF){
            if(!menu_state && ch == '\n' && menu_selection == 3) {
                return 0;
            }
            if(!menu_state && ch == '\n' && menu_selection == 0) {
                return 1;
            }
            if(!menu_state && ch == '\n' && menu_selection == 1) {
                menu_state = 1;
            }
            if(!menu_state && ch == 'w' && menu_selection != 0) {
                menu_selection--;
                change_menu_selection(menu_itens, limits[1], menu_selection);
                start_t2 = get_time() + 0.5;
            }
            if(!menu_state && ch == 's' && menu_selection != 3) {
                menu_selection++;
                change_menu_selection(menu_itens, limits[1], menu_selection);
                start_t2 = get_time() + 0.5;
            }
        }

        end_t = get_time();
        if(!menu_state && end_t - start_t2 > 0.3) {
            start_t2 = get_time();
            switch_selection_state(menu_itens, limits[1], menu_selection);
        }
        if(menu_state == 1) {
            for (int y = y_border_init + 1; y < y_border_fin - 1; y++)
                for (int x = x_border_init + 1; x < x_border_fin - 1; x++) {
                    if ((x == limits[2].start_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y) || (x == limits[2].end_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y))
                        print_coord(x, y, '|');
                    else if ((y == limits[2].start_y && x >= limits[2].start_x + 1 && x <= limits[2].end_x - 1) || (y == limits[2].end_y && x >= limits[2].start_x && x <= limits[2].end_x))
                        print_coord(x, y, '_');
                    else
                        print_coord(x, y, ' ');
                }
            limits[0].active = 0;
            limits[1].active = 0;
            limits[2].active = 1;
            menu_state++;
        }


        if(end_t - start_t > 0.1){
            start_t = get_time();
            for (int j = 0; j < 2; j++){
                for (int i = 0; i < stars_count; i++) {
                    if(!stars[i].visible){
                        int rn = rand_range(0, 3);
                        if(rn == 0 || rn == 1) {
                            stars[i].position.x = rand_range(x_border_init + 1, x_border_fin - 1);
                            stars[i].position.y = y_border_init + 1;
                        }
                        else if(rn == 2) {
                            stars[i].position.x = (x_border_fin + x_border_init) / 2 - 1;
                            stars[i].position.y = rand_range(y_border_init + 1, y_border_fin - 1);
                        }
                        else {
                            stars[i].position.x = (x_border_fin + x_border_init) / 2 + 1;
                            stars[i].position.y = rand_range(y_border_init + 1, y_border_fin - 1);
                        }

                        if(stars[i].position.x < limits[0].start_x + 20)
                            stars[i].side = -1;
                        else
                            stars[i].side = 1;
                        stars[i].old_position.x = stars[i].position.x;
                        stars[i].old_position.y = stars[i].position.y;
                        stars[i].visible = 1;
                        print_star(&stars[i], limits);
                        break;
                    }
                }
            }

            for (int i = 0; i < stars_count; i++) {
                if(stars[i].visible){
                    if(stars[i].position.y + 1 >= y_border_fin || stars[i].position.x - 1 <= x_border_init || stars[i].position.x + 1 >= x_border_fin){
                        stars[i].visible = 0;
                        print_coord(stars[i].position.x, stars[i].position.y, ' ');
                        continue;
                    }
                    stars[i].position.x += stars[i].side;
                    stars[i].position.y++;
                    print_star(&stars[i], limits);
                }
            }
        }
        usleep(1000);
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

    int m = menu();
    if(m == 1)
        main_game();


    printf("\e[?25h"); //ativar cursor
    printf("\033[0m"); //reset

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    system("clear");
}
