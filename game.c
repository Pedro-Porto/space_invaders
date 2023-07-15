#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "utils.h"
#include "game.h"
#include "menu.h"

int aliens_x = 11;
int aliens_y = 5;

ship_struct ship = {
    .sps = " |^| /(-)\\",
    .old_position = {0, 0},
    .invencible = 0,
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

char alien_img[4][20] = {
    " \\ / |O O|/( )\\", // alien 1
    " / \\ (O O)/^ ^\\", // alein 2
    "\\ | /     / | \\", // alien explodido
    "                 " // alien invisivel
};

// imprime a nave a partir de sua coordenada central
void print_ship(int color) {
    if(ship.invencible)
        color = 33;
    change_color(color, 0);
    if(ship.invencible && ship.invencible % 2) {
        for (int i = 0; i < 10; i++){
            print_coord(ship.old_position.x + (i - ((i / 5) * 5) - 2), ship.old_position.y - !(i / 5), ' ');
        }
    }
    else {
        for (int i = 0; i < 10; i++){
            if(ship.old_position.x != ship.position.x || ship.old_position.y != ship.position.y)
                print_coord(ship.old_position.x + (i - ((i / 5) * 5) - 2), ship.old_position.y - !(i / 5), ' ');
            print_coord(ship.position.x + (i - ((i / 5) * 5) - 2), ship.position.y - !(i / 5), ship.sps[i]);

        }
        ship.old_position.x = ship.position.x;
        ship.old_position.y = ship.position.y;
    }
}

// funcao para imprimir cada alien a partir de sua coordenada central
void print_alien(alien_struct *alien) {
    if(alien->visible){
        if(alien->type == 2)
            change_color(33, 0); //amarelo
        else if(alien->type == 0)
            change_color(36, 0); //ciano
        else
            change_color(32, 0); //verde
        if(alien->position.x != alien->old_position.x || alien->position.y != alien->old_position.y)
            for (int i = 0; i < 15; i++)
                print_coord(alien->old_position.x + (i - ((i / 5) * 5) - 2), alien->old_position.y + (i / 5) + 1, ' ');
        for (int i = 0; i < 15; i++)
            print_coord(alien->position.x + (i - ((i / 5) * 5) - 2), alien->position.y + (i / 5) + 1, alien_img[alien->type][i]);
        alien->old_position.x = alien->position.x;
        alien->old_position.y = alien->position.y;
    }
}

// altera a posicao para cada um dos tiros da nave
void print_shot() {
    for (int i = 0; i < 3; i++) {
        if(ship.shots[i].visible){
            if(ship.shots[i].position.y - 1 <= y_border_init){
                ship.shots[i].visible = 0;
                print_coord(ship.shots[i].position.x, ship.shots[i].position.y, ' ');
                continue;
            }
            ship.shots[i].position.y--;
            print_coord(ship.shots[i].old_position.x, ship.shots[i].old_position.y, ' ');
            change_color(36, 0); //azul
            print_coord(ship.shots[i].position.x, ship.shots[i].position.y, '|');
            ship.shots[i].old_position.x = ship.shots[i].position.x;
            ship.shots[i].old_position.y = ship.shots[i].position.y;
        }
        else {
            print_coord(ship.shots[i].position.x, ship.shots[i].position.y, ' ');
        }
    }
    
}

// cria os aliens
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

// move os aliens e caso tenha chegado na borda, vai para baixo e altera a direcao
void move_alien(int *alien_direction, alien_struct *aliens) {
    int al_max_coord = *alien_direction*1000; // 0 = direita; 1 = esquerda
    // acha a maior coordenada dos aliens
    for (int i = 0; i < aliens_y*aliens_x; i++) {
        if(*alien_direction == 0 && aliens[i].position.x > al_max_coord && aliens[i].visible)
            al_max_coord = aliens[i].position.x;
        else if(*alien_direction == 1 && aliens[i].position.x < al_max_coord && aliens[i].visible)
            al_max_coord = aliens[i].position.x;
    }
    
    if((!*alien_direction && al_max_coord + 5 >= x_border_fin) || (*alien_direction && al_max_coord - 5 <= x_border_init)){
        *alien_direction = !*alien_direction;
        for (int i = aliens_y*aliens_x - 1; i >= 0; i--) {
            aliens[i].position.y += 2;
            print_alien(&aliens[i]);
        }
    }
    else {
        if(*alien_direction){
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

// altera a quantidade de naves representando a vida em cima da tela
void change_life(int *life, ship_struct *ship) {
    coord old_position = ship->old_position;
    coord position = ship->position;
    for (int i = 2; i >= 0; i--) {
        if(*life - 1 >= i){
            ship->old_position = (coord){0, 0};
            ship->position = (coord){x_border_init + 15 + (i * 6), y_border_init + 2};
            print_ship(31);
        }
        else {
            ship->old_position = (coord){x_border_init + 15 + (i * 6), y_border_init + 2};
            ship->position = position;
            print_ship(37);
        }
    }
    ship->old_position = old_position;
    ship->position = position;
}

// escolhe um alien aleatorio (apenas os mais de baixo) e coloca um tiro em baixo dele
void create_alien_shot(shot_struct *alien_shot, alien_struct *aliens) {
    int column_count = 0;
    // acha os aliens disponiveis
    for (int x = 0; x < aliens_x; x++) {
        int visi_col = 0;
        for (int y = 0; y < aliens_y; y++) {
            if(aliens[y * aliens_x + x].visible){
                visi_col = 1;
                break;
            }  
        }
        column_count += visi_col;
    }
    int col = rand_range(0, column_count - 1);
    int max_y = 0;
    int max_x = 0;
    column_count = 0;
    int br = 0;
    // acha a poisicao do alien
    for (int x = 0; x < aliens_x; x++) {
        for (int y = aliens_y - 1; y >= 0; y--) {
            if(aliens[y * aliens_x + x].visible){
                if(col == column_count) {
                    max_y = aliens[y * aliens_x + x].position.y;
                    max_x = aliens[y * aliens_x + x].position.x;
                    br = 1;
                    break;
                }
                column_count++;
                break;
            } 
        }
        if(br)
            break;
    }
    
    // acha um tiro disponivel no array e coloca o tiro criado nele
    for (int i = 0; i < 10; i++) {
        if(!alien_shot[i].visible){
            alien_shot[i].visible = 1;
            alien_shot[i].position = (coord){max_x, max_y + 4};
            change_color(35, 0);
            print_coord(max_x, max_y + 4, '|');
            break;
        }
    }
}

// Move cada tiro dos aliens uma coordenada e verifica se houve colisão com a nave ou com a borda
void move_alien_shot(shot_struct *alien_shot, ship_struct *ship, int *lifes, double *start_t3) {
    for (int i = 0; i < 10; i++) {
        if(alien_shot[i].visible){
            change_color(35, 0);
            print_coord(alien_shot[i].position.x, alien_shot[i].position.y, ' ');
            if(!ship->invencible && alien_shot[i].position.x <= ship->position.x + 3 && alien_shot[i].position.x >= ship->position.x - 3 && alien_shot[i].position.y >= ship->position.y - 1 && alien_shot[i].position.y <= ship->position.y + 1){ // verifica colisao com a nave
                *lifes -= 1;
                change_life(lifes, ship);
                *start_t3 = get_time();
                ship->invencible = 1;
                alien_shot[i].visible = 0;
            }
            else if(alien_shot[i].position.y + 1 < y_border_fin){ // move para frente
                alien_shot[i].position.y++;
                print_coord(alien_shot[i].position.x, alien_shot[i].position.y, '|');
            }
            else { //chegou na borda
                alien_shot[i].visible = 0;
            }
            
        }
    }
    
}

// funcao de comparacao para o qsort
int compare(const void *a, const void *b) {
    score_struct *orderA = (score_struct *)a;
    score_struct *orderB = (score_struct *)b;
    return (orderB->points - orderA->points);
}

// salva a pontuacao caso esteja no high score
void save_score(int points) {
    score_struct h_scores[12];
    FILE *hs = fopen("highscore.txt", "r");
    if(hs == NULL){
        reset_terminal();
        visible_cusor(1);
        clear_screen();
        perror("Error in file: ");
        exit(1);
    }
    char player[20];
    int score;
    int count = 0;
    int inserted = 0;
    while (fscanf(hs, "%s %d", player, &score) != EOF) {
        if(strcmp(player, name)){
            strcpy(h_scores[count].player, player);
            h_scores[count].points = score;
        }
        else if(points > score){
            strcpy(h_scores[count].player, player);
            h_scores[count].points = points;
            inserted = 1;
        }
        else {
            strcpy(h_scores[count].player, player);
            h_scores[count].points = score;
            inserted = 1;
        }
        count++;
    }
    if(!inserted){
        strcpy(h_scores[count].player, name);
        h_scores[count].points = points;
        count++;
    }
    fclose(hs);
    qsort(h_scores, count, sizeof(score_struct), compare); // sort do maior para menor
    FILE *hsw = fopen("highscore.txt", "w");
    for (int i = 0; i < (count > 10 ? 10 : count); i++) { // salva no maximo 10 valores
        fprintf(hsw, "%s %d\n", h_scores[i].player, h_scores[i].points);
    }
    fclose(hsw);
}

// cria a tela de fim de jogo e verifica se apertou enter para voltar ao inicio
void end_game(int points) {
    fflush(stdout);
    print_tab();
    // definicao das bordas
    int start_x = (x_border_fin + x_border_init) / 2 - 25;
    int start_y = (y_border_fin + y_border_init) / 2 - 6;
    int end_x = start_x + 50;
    int end_y = start_y + 12;
    char go[20] = "Game over!";
    char po[20] = "Your score was:";
    char ct[25] = "Press enter to continue";

    for (int y = y_border_init; y < y_border_fin; y++)
        for (int x = x_border_init; x < x_border_fin; x++) {
                if ((x == start_x && y >= start_y + 1 && y <= end_y) || (x == end_x && y >= start_y + 1 && y <= end_y))
                    print_coord(x, y, '|');
                else if ((y == start_y && x >= start_x + 1 && x <= end_x - 1) || (y == end_y && x >= start_x && x <= end_x))
                    print_coord(x, y, '_');
            }
    change_color(36, 0);
    prints_coord((start_x + end_x)/2 - strlen(go) / 2, start_y + 2, go);
    prints_coord((start_x + end_x)/2 - strlen(po) / 2, start_y + 5, po);
    change_color(33, 0);
    printi_coord((start_x + end_x)/2 - intlen(points) / 2, start_y + 7, points);
    change_color(37, 41);
    prints_coord((start_x + end_x)/2 - strlen(ct) / 2, start_y + 10, ct);
    change_color(0, 0);
    fflush(stdout);
    msleep(10);
    save_score(points);
    while(1) {
        char ch = getchar();
        if(ch != EOF){
            if(ch == '\n') {
                return;
            }
        }
        msleep(10);
    }
}

// loop do jogo
void main_game() {
    double start_t, start_t2, start_t3, start_t4, start_t5, end_t; // contadores de tempo assincrono
    int ch;
    int alien_direction = 0; // 0 = direita; 1 = esquerda
    int points = 0; // pontuacao do jogador
    int lifes = 3; // vidas do jogador
    int alien_qtn; // quantidade de aliens vivos
    float alien_speed = 1.0; // velocidade de movimentaçao dos aliens
    int diff = 0; //dificuldade aumenta a cada rodada

    shot_struct alien_shot[10]; // array com os tiros dos aliens
    for (int i = 0; i < 10; i++) {
        alien_shot[i].visible = 0;
        alien_shot[i].old_position = (coord){0, 0};
    }

    print_tab();
    msleep(1);
    change_color(37, 0);
    printi_coord(x_border_init + 4, y_border_init + 2, points);
    change_life(&lifes, &ship);

    ship.position.x = (x_border_fin - x_border_init) / 2 + x_border_init;
    ship.position.y = y_border_fin - 3;

    print_ship(37);

    alien_struct aliens[aliens_y*aliens_x]; // array com os aliens e suas posicoes
    alien_constructor(aliens);

    fflush(stdout);
    start_t = get_time();
    start_t2 = get_time();
    start_t4 = get_time();
    while(1) {
        ch = getchar();
        if(ch != EOF){
            if(ch == 'a') { // nave para esquerda
                if(ship.position.x - 5 > x_border_init){
                    ship.position.x-=5;
                    print_ship(37);
                }
            }
            if(ch == 'd') { // nave para direita
                if(ship.position.x + 5 < x_border_fin){
                    ship.position.x+=5;
                    print_ship(37);
                }
            }
            if(ch == 'w') { //nave para cima
                if(ship.position.y - 3 > y_border_init){
                    ship.position.y-=2;
                    print_ship(37);
                }
            }
            if(ch == 's') { // nave para baixo
                if(ship.position.y + 2 < y_border_fin){
                    ship.position.y+=2;
                    print_ship(37);
                }
            }
            if(ch == 'x') // DEBUG: fecha o programa
                break;
            if(ch == ' '){ // tiro da nave
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
        if(end_t - start_t4 >= (0.6 - (double)((double)diff * 0.01))){ // tempo de criacao de um novo tiro do alien
            start_t4 = get_time();
            create_alien_shot(alien_shot, aliens);
        }
        if(end_t - start_t5 >= 0.1){ // tempo de movimentaçao do tiro do alien
            start_t5 = get_time();
            move_alien_shot(alien_shot, &ship, &lifes, &start_t3);
        }
        if(ship.invencible && end_t - start_t3 >= 0.3){ // tempo para a nave piscar quando esta invencivel
            ship.invencible++;
            start_t3 = get_time();
            if(ship.invencible == 10){
                ship.invencible = 0;
            }
            print_ship(37);
        }
        if(end_t - start_t >= 0.03) { // tempo do tiro da nave
            start_t = get_time();
            print_shot();
            alien_qtn = 0;
            for (int i = 0; i < aliens_y*aliens_x; i++) {
                if(aliens[i].visible)
                    alien_qtn++;
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
                        change_color(37, 0);
                        if(aliens[i].type == 0)
                            points += 50;
                        else
                            points += 20;
                        printi_coord(x_border_init + 4, y_border_init + 2, points);
                        print_shot();
                        aliens[i].type = 2;
                        print_alien(&aliens[i]);
                    }
                }
                if(!ship.invencible && aliens[i].type != 2 && aliens[i].type != 3 && aliens[i].visible && ship.position.x + 1 >= aliens[i].position.x - 2 && ship.position.x - 1 <= aliens[i].position.x + 2 && ship.position.y - 3 <= aliens[i].position.y + 1 && ship.position.y - 2 >= aliens[i].position.y - 1){
                    lifes--;
                    change_life(&lifes, &ship);
                    start_t3 = get_time();
                    ship.invencible = 1;
                }
            }
        }
        if(end_t - start_t2 >= alien_speed){ // tempo de movimentacao do alien
            start_t2 = get_time();
            move_alien(&alien_direction, aliens);
            if(alien_qtn > 50)
                alien_speed = 0.9;
            else if(alien_qtn > 30)
                alien_speed = 0.8;
            else if(alien_qtn > 20)
                alien_speed = 0.7;
            else if(alien_qtn > 5)
                alien_speed = 0.6;
            else if(alien_qtn > 1)
                alien_speed = 0.4;
            else 
                alien_speed = 0.2;
        }
        if(lifes <= 0) { // se acabar as vidas termina o loop
            break;
        }
        if(alien_qtn == 0) {
            alien_constructor(aliens);
            diff++;
        }
        msleep(1);
    }
    end_game(points); // tela de game over
}
