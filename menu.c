#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "menu.h"

char name[20];

void print_star(star_struct *star, limits_struct *limits) {
    int print_new = 1;
    int print_old = 1;
    for (int i = 0; i < 4; i++) {
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
        change_color(37, 0); //branco
        print_coord(star->position.x, star->position.y, star->side == 1 ? '\\' : '/');
    }
    star->old_position = star->position;
}

void change_menu_selection(menu_struct *menu_itens, limits_struct limits, int selection){
    for (int i = 0; i < 4; i++) {
        int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens->text_size / 2);
        int c_y = limits.start_y + (i * 3 + 3);
        if(i != selection){
            menu_itens->selected = 0;
            menu_itens->state = 1;
            change_color(37, 0); //branco
            for (int j = 0; j < menu_itens->text_size; j++) {
                print_coord(c_x + j, c_y, menu_itens->text[j]);
            }
            print_coord(c_x - 1, c_y, ' ');
            print_coord(c_x + menu_itens->text_size, c_y, ' ');
        }
        if(i == selection) {
            menu_itens->selected = 1;
            
            change_color(37, 41); //branco / fundo vermelho
            for (int j = 0; j < menu_itens->text_size; j++) {
                print_coord(c_x + j, c_y, menu_itens->text[j]);
            }
            change_color(37, 0); //branco
            print_coord(c_x - 1, c_y, '<');
            print_coord(c_x + menu_itens->text_size, c_y, '>');
        }
        menu_itens++;
    }
}

void switch_selection_state(menu_struct *menu_itens, limits_struct limits, int selection) {
    if(menu_itens[selection].state){
        change_color(37, 0); //branco
    }
    else {
        
        change_color(37, 41); //branco / fundo vermelho
    }
    menu_itens[selection].state = !menu_itens[selection].state;
    int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens[selection].text_size / 2);
    int c_y = limits.start_y + (selection * 3 + 3);
    for (int i = 0; i < menu_itens[selection].text_size; i++) {
        print_coord(c_x + i, c_y, menu_itens[selection].text[i]);
    }
}

void update_stars(int stars_count, star_struct *stars, limits_struct *limits) {
    change_color(37, 0);
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

void build_init(limits_struct *limits, menu_struct *menu_itens) {
    char title[450] = "       ____                                    / ___| _ __   __ _  ___ ___              \\___ \\| '_ \\ / _` |/ __/ _ \\              ___) | |_) | (_| | (_|  __/             |____/| .__/ \\__,_|\\___\\___|        ___        |_|          _               |_ _|_ ____   ____ _  __| | ___ _ __ ___  | || '_ \\ \\ / / _` |/ _` |/ _ \\ '__/ __| | || | | \\ V / (_| | (_| |  __/ |  \\__ \\|___|_| |_|\\_/ \\__,_|\\__,_|\\___|_|  |___/";
    // 10 x 41
    limits[0].start_x = (x_border_fin + x_border_init) / 2 - 20;
    limits[0].start_y = y_border_init + 5;
    limits[0].end_x = limits[0].start_x + 41;
    limits[0].end_y = limits[0].start_y + 10;
    limits[0].active = 1;
    change_color(36, 0); //ciano
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 41; j++) {
            print_coord(limits[0].start_x + j, limits[0].start_y + i, title[i*41 + j]);
        }
    }
    change_color(31, 0); //vermelho
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
    change_color(37, 0); //branco
    for (int i = 0; i < 4; i++){
        int c_x = (x_border_fin + x_border_init) / 2 - (menu_itens[i].text_size / 2);
        int c_y = limits[1].start_y + (i * 3 + 3);
        for (int j = 0; j < menu_itens[i].text_size; j++) {
            print_coord(c_x + j, c_y, menu_itens[i].text[j]);
        }
        
    }
    limits[2].active = 0;
}

void build_highscore(limits_struct *limits) {
    for (int y = y_border_init + 1; y < y_border_fin - 1; y++)
        for (int x = x_border_init + 1; x < x_border_fin - 1; x++) {
            if ((x == limits[2].start_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y) || (x == limits[2].end_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y))
                print_coord(x, y, '|');
            else if ((y == limits[2].start_y && x >= limits[2].start_x + 1 && x <= limits[2].end_x - 1) || (y == limits[2].end_y && x >= limits[2].start_x && x <= limits[2].end_x))
                print_coord(x, y, '_');
        }
    fflush(stdout);
    limits[0].active = 0;
    limits[1].active = 0;
    limits[2].active = 1;
    print_coord(limits[2].start_x + 2, limits[2].start_y + 2, ' ');
    change_color(37, 41);
    printf("< back");
    change_color(0, 0);
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
    int count = 1;
    while (fscanf(hs, "%s %d", player, &score) != EOF) {
        int len = strlen(player) + intlen(score) + 1;
        print_coord(((limits[2].start_x + limits[2].end_x) / 2) - (len / 2) - 1, count * 4 + limits[2].start_y - 2, ' ');
        change_color(37, 0);
        printf("%s", player);
        change_color(36, 0);
        printf(" %d", score);
        count++;
    }
    fclose(hs);
}

void back_switch_selection_state(int *state, limits_struct *limits) {
    change_color(0, 0);
    print_coord(limits[2].start_x + 2, limits[2].start_y + 2, ' ');
    if(*state){
        change_color(37, 0); //branco
    }
    else {
        change_color(37, 41); //branco / fundo vermelho
    }
    *state = !*state;
    printf("< back");
}

void build_rules(limits_struct *limits) {
    for (int y = y_border_init + 1; y < y_border_fin - 1; y++)
        for (int x = x_border_init + 1; x < x_border_fin - 1; x++) {
            if ((x == limits[2].start_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y) || (x == limits[2].end_x && y >= limits[2].start_y + 1 && y <= limits[2].end_y))
                print_coord(x, y, '|');
            else if ((y == limits[2].start_y && x >= limits[2].start_x + 1 && x <= limits[2].end_x - 1) || (y == limits[2].end_y && x >= limits[2].start_x && x <= limits[2].end_x))
                print_coord(x, y, '_');
        }
    fflush(stdout);
    limits[0].active = 0;
    limits[1].active = 0;
    limits[2].active = 1;
    print_coord(limits[2].start_x + 2, limits[2].start_y + 2, ' ');
    change_color(37, 41);
    printf("< back");
    change_color(0, 0);
}

void build_name_input(limits_struct *limits) {
    for (int y = y_border_init + 1; y < y_border_fin - 1; y++)
        for (int x = x_border_init + 1; x < x_border_fin - 1; x++) {
            if ((x == limits[3].start_x && y >= limits[3].start_y + 1 && y <= limits[3].end_y) || (x == limits[3].end_x && y >= limits[3].start_y + 1 && y <= limits[3].end_y))
                print_coord(x, y, '|');
            else if ((y == limits[3].start_y && x >= limits[3].start_x + 1 && x <= limits[3].end_x - 1) || (y == limits[3].end_y && x >= limits[3].start_x && x <= limits[3].end_x))
                print_coord(x, y, '_');
        }
    fflush(stdout);
    limits[0].active = 0;
    limits[1].active = 0;
    limits[3].active = 1;
    print_coord(limits[3].start_x + 2, limits[3].start_y + 2, ' ');
    change_color(36, 0);
    printf("Insert your name:");
    change_color(33, 0);
    print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9, (limits[3].start_y + limits[3].end_y) / 2 + 1, '-');
    printf("------------------");
    
}

void cursor_name_input(int *state, int loc, limits_struct *limits){
    change_color(37, 0);
    if(*state)
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9 + loc, (limits[3].start_y + limits[3].end_y) / 2, '|');
    else
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9 + loc, (limits[3].start_y + limits[3].end_y) / 2, ' ');
    *state = !*state;
}

void name_input(char c, int *loc, limits_struct *limits) {
    change_color(37, 0);
    c = c == ' ' ? '_' : c;
    if((c == '\b' || c == 127) && *loc > 0) {
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9 + *loc, (limits[3].start_y + limits[3].end_y) / 2, ' ');
        *loc = *loc - 1;
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9 + *loc, (limits[3].start_y + limits[3].end_y) / 2, '|');
        name[*loc] = '\0';
    }
    else if(c >= 33 && c <= 126 && *loc < 19) {
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 9 + *loc, (limits[3].start_y + limits[3].end_y) / 2, c);
        name[*loc] = c;
        name[*loc + 1] = '\0';
        *loc = *loc + 1;
    }
    if(*loc == 0) {
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 11, (limits[3].start_y + limits[3].end_y) / 2 + 4, ' ');
        change_color(0, 0);
        printf("                       ");
    }
    else {
        print_coord((limits[3].start_x + limits[3].end_x) / 2 - 11, (limits[3].start_y + limits[3].end_y) / 2 + 4, ' ');
        change_color(37, 41);
        printf("Press enter to continue");
    }
}

int menu() {
    int ch;
    int stars_count = 150;
    int menu_selection = 0;
    int menu_state = 0; // 0 = init | 1 = highscore | 2 = rules | 3 = name input
    int hs_selec_state = 1;
    int ni_state = 1;
    int ni_loc = 0;
    double start_t, start_t2, end_t;
    star_struct stars[stars_count];
    limits_struct limits[4] = {
        {
            .active = 0
        },
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
            .text = "Rules",
            .text_size = 5,
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
    print_tab();
    build_init(limits, menu_itens);
    
    
    limits[2].start_x = (x_border_fin + x_border_init) / 2 - 40;
    limits[2].start_y = (y_border_fin + y_border_init) / 2 - 20;
    limits[2].end_x = limits[2].start_x + 80;
    limits[2].end_y = limits[2].start_y + 40;

    limits[3].start_x = (x_border_fin + x_border_init) / 2 - 25;
    limits[3].start_y = (y_border_fin + y_border_init) / 2 - 6;
    limits[3].end_x = limits[3].start_x + 50;
    limits[3].end_y = limits[3].start_y + 12;
    

    change_menu_selection(menu_itens, limits[1], menu_selection); //muda a opcao que fica selecionada no menu
    fflush(stdout);
    start_t = get_time(); //contador de tempo para as estrelas
    start_t2 = start_t; //contador de tempo para o cursor que pisca do menu
    while(1){
        ch = getchar();
        if(ch != EOF){
            if(menu_state == 0 && ch == '\n' && menu_selection == 3) {
                return 0;
            }
            else if(menu_state == 0 && ch == '\n' && menu_selection == 0) {
                print_tab();
                build_name_input(limits);
                menu_state = 3;
                //return 1;
            }
            else if(menu_state == 0 && ch == '\n' && menu_selection == 1) {
                print_tab();
                build_highscore(limits);
                menu_state = 1;
            }
            else if(menu_state == 0 && ch == '\n' && menu_selection == 2) {
                print_tab();
                build_rules(limits);
                menu_state = 2;
            }
            else if(menu_state == 0 && ch == 'w' && menu_selection != 0) {
                menu_selection--;
                change_menu_selection(menu_itens, limits[1], menu_selection);
                start_t2 = get_time() + 0.5;
            }
            else if(menu_state == 0 && ch == 's' && menu_selection != 3) {
                menu_selection++;
                change_menu_selection(menu_itens, limits[1], menu_selection);
                start_t2 = get_time() + 0.5;
            }
            else if(menu_state == 1 && ch == '\n') {
                print_tab();
                build_init(limits, menu_itens);
                change_menu_selection(menu_itens, limits[1], menu_selection);
                menu_state = 0;
            }
            else if(menu_state == 2 && ch == '\n') {
                print_tab();
                build_init(limits, menu_itens);
                change_menu_selection(menu_itens, limits[1], menu_selection);
                menu_state = 0;
            }
            else if(menu_state == 3 && ch == '\n' && ni_loc > 0) {
                return 1;
            }
            else if(menu_state == 3) {
                name_input(ch, &ni_loc, limits);
            }
        }

        end_t = get_time();
        if(end_t - start_t2 > 0.3) {
            start_t2 = get_time();
            if(menu_state == 0)
                switch_selection_state(menu_itens, limits[1], menu_selection);
            else if (menu_state == 1 || menu_state == 2)
                back_switch_selection_state(&hs_selec_state, limits);
            else if (menu_state == 3)
                cursor_name_input(&ni_state, ni_loc, limits);
        }

        if(end_t - start_t > 0.1){
            start_t = get_time();
            update_stars(stars_count, stars, limits);
        }
        msleep(1);
    }
}
