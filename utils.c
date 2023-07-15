#include<sys/ioctl.h>
#include<unistd.h>
#include<sys/time.h>
#include<time.h>
#include<termios.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<math.h>
#include "utils.h"

coord ws;
int x_border_init;
int x_border_fin;
int y_border_init;
int y_border_fin;
int x_size = 150;
int y_size = 45;
struct termios oldt;
int oldf;

// retorna o tamanho da janela do terminal em caracteres
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

// numero aleatorio dentro de um range de numeros
int rand_range(int minimum_number, int max_number){
    unsigned long seed = mix(clock(), time(NULL), getpid());
    srand(seed);
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}

// retorna o tempo do sistema em segundos
double get_time() {
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    double t = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
    return t/1000;
}

// imprime um caractere na coordenada da tela
void print_coord(int x, int y, char c) {
    printf("\033[%d;%dH%c", y + 1, x + 1, c);
}

// imprime uma string na coordenada da tela
void prints_coord(int x, int y, char *c) {
    printf("\033[%d;%dH%s", y + 1, x + 1, c);
}

// imprime um int na coordenada da tela
void printi_coord(int x, int y, int i) {
    printf("\033[%d;%dH%d", y + 1, x + 1, i);
}

// muda a cor do texto do terminal
void change_color(int color, int bg) {
    /*
    Foreground:
        30 Black
        31 Red
        32 Green
        33 Yellow
        34 Blue
        35 Magenta
        36 Cyan
        37 White
    Background:
        40 Black
        41 Red
        42 Green
        43 Yellow
        44 Blue
        45 Magenta
        46 Cyan
        47 White
    0 Reset all
    1 Bold
    */
    if(bg != 0)
        printf("\033[%d;%dm", color, bg);
    else
        printf("\033[0;%dm", color);
}

// define as bordas do jogo (#)
void define_borders() {
    ws = window_size();
    x_border_init = (ws.x - x_size) / 2;
    x_border_fin = x_border_init + x_size;
    y_border_init = (ws.y - y_size) / 2;
    y_border_fin = y_border_init + y_size;
}


// altera o estado do cursor entre visivel e invisivel
void visible_cusor(int state) {
    if(!state)
        printf("\e[?25l"); //desativar cursor
    else
        printf("\e[?25h"); //ativar cursor
}

// define o stdin como non block (nao espera a tecla para retornar)
void nonblock_terminal() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
}

// coloca o terminal no modo padrao
void reset_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
}

// limpa a tela
void clear_screen() {
    system("clear");
}

// agurda o tempo definido em milisegundos
void msleep(int millis) {
    usleep(millis * 1000);
}

// imprime as bordas do jogo
void print_tab() {
    clear_screen();
    define_borders();
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

// retorna o tamnho de um int caso ele fosse uma string
int intlen(int i) {
    return floor(log10(abs(i))) + 1;
}
