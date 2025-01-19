#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "music.h"

struct point{
    int x;
    int y;
    int full;
};

typedef struct{
    char ch;
    int color_pair;
    int color_check;
} character;

int d;
character **map;
int room_count;
struct point doors[50];
struct room rooms[12];
int current_room = 0;
int start_check;
int gold;
int strength = 20;
struct point staircases[4];
int food[4] ={0, 0, 0, 0};
int hunger = 0;
int backpack[5] ={1, 0, 0, 0, 0};
int default_weapon = 0;
int spells[3] ={0, 0, 0};
int ancient_key_count = 0;
struct point ancient_key;
long int start_time;

struct player{
    int x;
    int y;
    character under;
};

struct player player;

void print_map_with_colors(int floor_num);
int random_with_range(int min_rand, int max_rand){
    return ((rand() % (max_rand - min_rand + 1)) + min_rand);
}

struct password_door{
    int x;
    int y;
    int password;
    int x_button;
    int y_button;
    int unlocked;
};

struct password_door password_doors[3];
int password_doors_count = 3;

int find_password_door(int x, int y){
    for(int i = 0; i < 3; i++){
        if(password_doors[i].x == x && password_doors[i].y == y){
            return i;
        }
    }
    return -1;
}

int find_door_to_button(int x_button, int y_button){
    for(int i = 0; i < 3; i++){
        if(abs(password_doors[i].x - x_button) == 1 && abs(password_doors[i].y - y_button) == 1){
            return i;
        }
    }
}

void ask_password(int password, int x, int y, int floor_num){
    int password_door_num = find_password_door(x, y);

    print_map_with_colors(floor_num);
    initscr();
    start_color();
    keypad(stdscr, TRUE);

    WINDOW *win = newwin(5, 30, (LINES - 5) / 2, (COLS - 30) / 2);
    keypad(win, TRUE);
    box(win, 0, 0);
    if(password == -1){
        int highlight = 0;
        char *options[2] ={"Use Ancient Key", "Back"};
        while(1){
            box(win, 0, 0);
            mvwprintw(win, 1, 3, "Door has no Password Yet.");
            
            for(int i = 0; i < 2; i++){
                if(highlight == i){
                    wattron(win, A_REVERSE);
                }
                else{
                    wattroff(win, A_REVERSE);
                }
                mvwprintw(win, 3, i == 0 ? 2 : 20, "%s", options[i]);
            }
            wattroff(win, A_REVERSE);

            int c = wgetch(win);
            if(c == KEY_RIGHT && highlight == 0){
                highlight++;
            }
            else if(c == KEY_LEFT && highlight == 1){
                highlight--;
            }
            else if(c == 10){
                if(highlight == 0){
                    if(ancient_key_count <= 1){
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, 2, 1, "You Are Out of Ancient Keys.");
                        wattroff(win, COLOR_PAIR(1));
                        wrefresh(win);
                        wgetch(win);
                        break;
                    }
                    else{
                        if(rand() % 10 == 0){
                            wattron(win, COLOR_PAIR(1));
                            mvwprintw(win, 2, 6, "Ancient Key Broke.");
                            wattroff(win, COLOR_PAIR(1));
                            ancient_key_count--;
                        }
                        else{
                            wattron(win, COLOR_PAIR(10));
                            mvwprintw(win, 2, 2, "Door Unlocked Successfully.");
                            wattroff(win, COLOR_PAIR(10));
                            ancient_key_count -= 2;
                            password_doors[password_door_num].unlocked = 1;
                        }
                        
                        wrefresh(win);
                        wgetch(win);
                        break;
                    }
                }
                else if(highlight == 1){
                    wattroff(win, A_REVERSE);
                    refresh();
                    break;
                }
            }
            wrefresh(win);
        }
        delwin(win);
        noecho();
        
        endwin();
    }
    else{
        int highlight = 0;
        while(1){
            mvwprintw(win, 1, 2, "How do You Open the Door?");
            char *options[2] ={"Use Ancient Key", "Use Password"};
            
            for(int i = 0; i < 2; i++){
                if(highlight == i){
                    wattron(win, A_REVERSE);
                }
                else{
                    wattroff(win, A_REVERSE);
                }
                mvwprintw(win, i + 2, (30 - strlen(options[i])) / 2, "%s", options[i]);
            }
            wattroff(win, A_REVERSE);

            int c = wgetch(win);
            if(c == KEY_DOWN && highlight == 0){
                highlight++;
            }
            else if(c == KEY_UP && highlight == 1){
                highlight--;
            }

            if(c == 10){
                wclear(win);
                box(win, 0, 0);
                if(highlight == 0){
                    if(ancient_key_count <= 1){
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, 2, 1, "You Are Out of Ancient Keys.");
                        wattroff(win, COLOR_PAIR(1));
                        wrefresh(win);
                        wgetch(win);
                        break;
                    }
                    else{
                        if(rand() % 2 == 0){
                            wattron(win, COLOR_PAIR(1));
                            mvwprintw(win, 2, 6, "Ancient Key Broke.");
                            wattroff(win, COLOR_PAIR(1));
                            ancient_key_count--;
                        }
                        else{
                            wattron(win, COLOR_PAIR(10));
                            mvwprintw(win, 2, 2, "Door Unlocked Successfully.");
                            wattroff(win, COLOR_PAIR(10));
                            ancient_key_count -= 2;
                            password_doors[password_door_num].unlocked = 1;
                        }
                        
                        wrefresh(win);
                        wgetch(win);
                        break;
                    }
                }
                if(highlight == 1){
                    mvwprintw(win, 1, 5, "Enter Password: ");
                    echo();
                    curs_set(1);
                    wrefresh(win);

                    int input;
                    int tries = 0;
                    while(tries < 3){
                        echo();
                        curs_set(1);
                        mvwprintw(win, 3, 1, "                            ");
                        mvwprintw(win, 1, 21, "    ");
                        mvwscanw(win, 1, 21, "%d", &input);
                        curs_set(0);
                        noecho();
                        if(input == password){
                            wattron(win, COLOR_PAIR(10));
                            mvwprintw(win, 3, 12, "Correct");
                            password_doors[password_door_num].unlocked = 1;
                            wattroff(win, COLOR_PAIR(10));
                            wrefresh(win);
                            wgetch(win);
                            delwin(win);
                            noecho();
                            endwin();
                            return;
                        }
                        else{
                            noecho();
                            if(tries == 0){
                                wattron(win, COLOR_PAIR(11));
                                mvwprintw(win, 3, 6, "Wrong (First Try)");
                                wattroff(win, COLOR_PAIR(11));
                                wrefresh(win);
                                wgetch(win);
                            }
                            if(tries == 1){
                                wattron(win, COLOR_PAIR(12));
                                mvwprintw(win, 3, 6, "Wrong (Second Try)");
                                wattroff(win, COLOR_PAIR(12));
                                wrefresh(win);
                                wgetch(win);
                            }
                            if(tries == 2){
                                wattron(win, COLOR_PAIR(1));
                                mvwprintw(win, 3, 3, "Security Mode Activated.");
                                wattroff(win, COLOR_PAIR(1));
                                wrefresh(win);
                                wgetch(win);
                                delwin(win);
                                noecho();
                                
                                endwin();
                                return;
                            }
                            tries++;
                        }
                    }
                }
            }
            wrefresh(win);
        }
        delwin(win);
        noecho();
        
        endwin();
    }
}

int check_movement(int floor_num, int x, int y){
    chtype ch = mvinch(y, x);
    char character = ch & A_CHARTEXT;

    if(character == '@' && (ch & A_COLOR) == COLOR_PAIR(10)){
        return 1;
    }

    if(character == 'f' && (ch & A_COLOR) == COLOR_PAIR(14)){
        food[0]++;
    }
    if(character == 'f' && (ch & A_COLOR) == COLOR_PAIR(7)){
        food[1]++;
    }
    if(character == 'f' && (ch & A_COLOR) == COLOR_PAIR(13)){
        food[2]++;
    }
    if(character == 'f' && (ch & A_COLOR) == COLOR_PAIR(10)){
        food[3]++;
    }

    if(character == '$' && (ch & A_COLOR) == COLOR_PAIR(10)){
        spells[0]++;
    }
    if(character == '$' && (ch & A_COLOR) == COLOR_PAIR(18)){
        spells[1]++;
    }
    if(character == '$' && (ch & A_COLOR) == COLOR_PAIR(1)){
        spells[2]++;
    }
    
    if(character == '<' || character == '^' || character == '*' || character == '$' || character == 'M' || character == 'D' || character == 'W' || character == 'A' || character == 'S' || character == 'x' || character == '+' || character == '#' || character == '.' || character == '&' || character == 'f'){
        return 1;
    }

    if(ancient_key.x == x && ancient_key.y == y){
        return 1;
    }

    if(character == '@'){
        attron(COLOR_PAIR(5));
    
        int password_door_num = find_password_door(x, y);
        ask_password(password_doors[password_door_num].password, x, y, floor_num);
        mvprintw(player.y, player.x, "%c", player.under.ch);
    }

    return 0;
}

int is_top_left(int x, int y){
    if(mvinch(y, x) == 'U' || mvinch(y, x) == 'E' && mvinch(y + 1, x) == '|' && mvinch(y, x + 1) == '_'){
        return 1;
    }
    return 0;
}

void print_rooms(){
    for(int i = 0; i < room_count; i++){
        if(rooms[i].theme == 1){
            attron(COLOR_PAIR(21));
        }
        if(rooms[i].theme == 2){
            attron(COLOR_PAIR(9));
        }

        if(rooms[i].explored == 0){
            attron(COLOR_PAIR(20));
        }
        else{
            attroff(COLOR_PAIR(20));
        }

        int x_start = rooms[i].x_top_left;
        int y_start = rooms[i].y_top_left;
        int width = rooms[i].x_size;
        int height = rooms[i].y_size;

        mvprintw(y_start, x_start, "_");
        for(int j = 1; j <= width; j++){
            move(y_start, x_start + j);
            printw("_");
        }
        mvprintw(y_start, x_start + width + 1, "_");

        for(int j = 1; j <= height; j++){
            move(y_start + j, x_start);
            printw("|");
            for(int k = 1; k <= width; k++){
                mvprintw(y_start + j, x_start + k, ".");
            }
            move(y_start + j, x_start + width + 1);
            printw("|");
        }

        mvprintw(y_start + height + 1, x_start, "_");
        for(int j = 1; j <= width; j++){
            move(y_start + height + 1, x_start + j);
            printw("_");
        }
        mvprintw(y_start + height + 1, x_start + width + 1, "_");

        attroff(COLOR_PAIR(20));
    }

    attron(COLOR_PAIR(5));
    mvprintw(player.y, player.x, "\U0001FBC5");
    attroff(COLOR_PAIR(5));
}

int find_room(int x, int y){
    for(int i = 0; i < room_count; i++){
        if(x >= rooms[i].x_top_left && x <= rooms[i].x_top_left + rooms[i].x_size + 1 &&
            y >= rooms[i].y_top_left && y <= rooms[i].y_top_left + rooms[i].y_size + 1){
            return i;
        }
    }
    return -1;
}

void generate_pillars(){
    int pillar_count = rand_with_range(10, 20);
    int i = 0;
    int x, y;
    while(i < pillar_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            map[y][x].color_pair = 21;
            if(rooms[find_room(x, y)].explored == 0){
                attron(COLOR_PAIR(20));
            }
            mvprintw(y, x, "O");
            map[y][x].ch = 'O';
            i++;
        }
    }
}

void generate_staircase(int floor_num){
    int i = 0;
    int x, y;
    while(i < 1){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            if(rooms[find_room(x, y)].explored == 0){
            staircases[floor_num].x = x;
            staircases[floor_num].y = y;
            i++;
            }
        }
    }
}

void generate_gold(){
    int gold_count = rand_with_range(10, 20);
    int i = 0;
    int x, y;
    
    while(i < gold_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        attron(COLOR_PAIR(20));
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            
            if(rand() % 10 == 0){
                if(rooms[find_room(x, y)].explored == 1){
                    attron(COLOR_PAIR(15));
                }
                mvprintw(y, x, "x");
                map[y][x].color_pair = 15;
                attroff(COLOR_PAIR(15));
            }
            else{
                if(rooms[find_room(x, y)].explored == 1){
                    attron(COLOR_PAIR(7));
                }
                mvprintw(y, x, "*");
                map[y][x].color_pair = 7;
                attroff(COLOR_PAIR(7));
            }
            i++;
        }
    }
}

void generate_food(){
    int food_count = rand_with_range(10 - 2 * d, 20 - 2 * d);
    int i = 0;
    int x, y;
    while(i < food_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            int random = rand() % 6;
            if(random < 3){
                attron(COLOR_PAIR(14));
                map[y][x].color_pair = 14;
            }
            if(random == 3){
                attron(COLOR_PAIR(7));
                map[y][x].color_pair = 7;
            }
            if(random == 4){
                attron(COLOR_PAIR(13));
                map[y][x].color_pair = 13;
            }
            if(random == 5){
                attron(COLOR_PAIR(10));
                map[y][x].color_pair = 10;
            }
            if(rooms[find_room(x, y)].explored == 0){
                attron(COLOR_PAIR(20));
            }
            mvprintw(y, x, "f");
            map[y][x].ch = 'f';
            i++;
        }
    }
}

void generate_weapon(){
    int weapon_count = rand_with_range(5, 10);
    int i = 0;
    int x, y;
    attron(COLOR_PAIR(16));
    while(i < weapon_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            map[y][x].color_pair = 16;
            if(rooms[find_room(x, y)].explored == 0){
                attron(COLOR_PAIR(20));
            }
            int random = rand() % 6;
            if(random == 0){
                mvprintw(y, x, "M");
                map[y][x].ch = 'M';
            }
            if(random == 1){
                mvprintw(y, x, "D");
                map[y][x].ch = 'D';
            }
            if(random == 2){
                mvprintw(y, x, "W");
                map[y][x].ch = 'W';
            }
            if(random == 3){
                mvprintw(y, x, "A");
                map[y][x].ch = 'A';
            }
            if(random == 4){
                mvprintw(y, x, "S");
                map[y][x].ch = 'S';
            }
            i++;
        }
    }
    attroff(COLOR_PAIR(16));
}

void generate_spell(){
    int spell_count = rand_with_range(5, 10);
    int i = 0;
    int x, y;
    while(i < spell_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            int random = rand() % 3;
            if(random == 0){
                attron(COLOR_PAIR(10));
                map[y][x].color_pair = 10;
            }
            if(random == 1){
                attron(COLOR_PAIR(18));
                map[y][x].color_pair = 18;
            }
            if(random == 2){
                attron(COLOR_PAIR(1));
                map[y][x].color_pair = 1;
            }
            if(rooms[find_room(x, y)].explored == 0){
                attron(COLOR_PAIR(20));
            }
            mvprintw(y, x, "$");
            map[y][x].ch = '$';
            i++;
        }
    }

    int j = 0;
    for(int i = 0; i < room_count; i++){
        if(rooms[i].theme == 2){
            int j = 0;
            int count = 0;
            while(j < 10 && count < 15){
                int x = rooms[i].x_top_left + rand_with_range(1, rooms[i].x_size);
                int y = rooms[i].y_top_left + rand_with_range(1, rooms[i].y_size);
                if((mvinch(y, x) & A_CHARTEXT) == '.'){
                    int random = rand() % 3;
                    if(random == 0){
                        attron(COLOR_PAIR(10));
                        map[y][x].color_pair = 10;
                    }
                    else if(random == 1){
                        attron(COLOR_PAIR(18));
                        map[y][x].color_pair = 18;
                    }
                    else if(random == 2){
                        attron(COLOR_PAIR(1));
                        map[y][x].color_pair = 1;
                    }
                    if(rooms[find_room(x, y)].explored == 0){
                        attron(COLOR_PAIR(20));
                    }
                    mvprintw(y, x, "$");
                    map[y][x].ch = '$';
                    j++;
                }
                count++;
            }
        }
    }
}

void generate_ancient_key(){
    attron(COLOR_PAIR(7));
    int i = 0;
    int x, y;
    int line = 0;
    while(i < 1){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.'){
            if(rooms[find_room(x, y)].theme == 1){
                ancient_key.x = x;
                ancient_key.y = y;
                ancient_key.full = 1;
                i++;
            }
        }
    }
    attroff(COLOR_PAIR(7));
}

int check_trap(int x, int y, struct point *traps, int trap_count, int *strength){
    for(int i = 0; i < trap_count; i++){
        if(traps[i].x == x && traps[i].y == y){
            mvprintw(0, 0, "You Have Stepped on a Trap.");
            (*strength)--;
            getch();
            mvprintw(0, 0, "                           ");
            return i;
        }
    }
    return -1;
}

void display_door_password(int password, int floor_num){
    attron(COLOR_PAIR(5));
    mvprintw(player.y, player.x, "\U0001FBC5");
    attroff(COLOR_PAIR(5));
    
    WINDOW *win = newwin(5, 30, (LINES - 5) / 2, (COLS - 30) / 2);
    keypad(win, TRUE);
    nodelay(win, TRUE);

    clear();
    print_map_with_colors(floor_num);
    for(int i = 30; i >= 0; i--){
        box(win, 0, 0);
        mvwprintw(win, 1, (30 - 14) / 2, "Password: %d", password);
        wattron(win, A_REVERSE);
        mvwprintw(win, 3, (30 - strlen("Ok")) / 2, "Ok");
        wattroff(win, A_REVERSE);
        mvwprintw(win, 2, (30 - 21) / 2, "Time left: %2d seconds", i);
        wrefresh(win);

        for(int j = 0; j < 100; j++){
            usleep(10000);
            int ch = wgetch(win);
            if(ch == '\n'){
                delwin(win);
                refresh();
                return;
            }
        }
    }

    delwin(win);
    refresh();
}

void food_list(int *strength){
    typedef struct{
        char ch;
        int color_pair;
    } character;

    character map[LINES][COLS];
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            map[y][x].ch = (mvinch(y, x) & A_CHARTEXT);
            map[y][x].color_pair = PAIR_NUMBER(mvinch(y, x) & A_COLOR);
        }
    }

    int highlight = 0;
    int check = 0;

    char *options[2] ={"Eat 1 Normal Food", "Back"};
    while(!check){
        clear();
        attroff(A_REVERSE);
        attron(A_BOLD);
        mvprintw(LINES / 2 - 6, (COLS - 11) / 2, "Food List: ");
        attroff(A_BOLD);

        attron(COLOR_PAIR(14));
        mvprintw(LINES / 2 - 4, (COLS - 16) / 2, "Normal Food: %d", food[0]);
        attron(COLOR_PAIR(7));
        mvprintw(LINES / 2 - 2, (COLS - 16) / 2, "Deluxe Food: %d", food[1]);
        attron(COLOR_PAIR(13));
        mvprintw(LINES / 2, (COLS - 16) / 2, "Magical Food: %d", food[2]);
        attron(COLOR_PAIR(10));
        mvprintw(LINES / 2 + 2, (COLS - 16) / 2, "Rotten Food: %d", food[3]);
        attroff(COLOR_PAIR(10));

        mvprintw(LINES / 2 + 4, (COLS - 30) / 2, "Hunger ");
        if(hunger < 20){
            attron(COLOR_PAIR(1));
            for(int i = 0; i < hunger; i++){
                printw("#");
            }
            attroff(COLOR_PAIR(1));
            for(int i = hunger; i < 20; i++){
                printw("#");
            }
        }
        else{
            attron(COLOR_PAIR(1));
            for(int i = 0; i < 20; i++){
                printw("#");
            }
            attroff(COLOR_PAIR(1));
        }

        for(int i = 0; i < 2; i++){
            if(highlight == i){
                attron(A_REVERSE);
            }
            else{
                attroff(A_REVERSE);
            }
            mvprintw(LINES / 2 + 6 + 2 * i, (COLS - strlen(options[i])) / 2, "%s", options[i]);
        }

        int c = getch();
        if(c == KEY_DOWN && highlight == 0){
            highlight++;
        }
        if(c == KEY_UP && highlight == 1){
            highlight--;
        }
        if(c == 10){
            if(highlight == 0){
                if(hunger == 0){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "You Are Not Hungry. Press Any Key to Continue...");
                    attroff(COLOR_PAIR(1));
                    getch();
                }
                else if(food[0] > 0){
                    food[0]--;
                    hunger--;
                    (*strength)++;
                    start_time = time(NULL);
                }
                else{
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "You Are Out of Food. Press Any Key to Continue...");
                    attroff(COLOR_PAIR(1));
                    getch();
                }
            }
            else if(highlight == 1){
                attroff(A_REVERSE);
                refresh();
                clear();
                check = 1;
            }
        }
    }

    clear();
    refresh();
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            attron(COLOR_PAIR(map[y][x].color_pair));
            mvprintw(y, x, "%c", map[y][x].ch);
            attroff(COLOR_PAIR(map[y][x].color_pair));
        }
    }
    refresh();
}

void weapon_list(){
    character map[LINES][COLS];
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            map[y][x].ch = (mvinch(y, x) & A_CHARTEXT);
            map[y][x].color_pair = PAIR_NUMBER(mvinch(y, x) & A_COLOR);
        }
    }

    while(1){
        clear();
        attroff(A_REVERSE);
        attron(A_BOLD);
        mvprintw(LINES / 2 - 6, (COLS - 11) / 2, "Backpack: ");
        attroff(A_BOLD);

        mvprintw(LINES / 2 - 4, (COLS - 16) / 2, "Mace: %d", backpack[0]);
        mvprintw(LINES / 2 - 2, (COLS - 16) / 2, "Dagger: %d", backpack[1]);
        mvprintw(LINES / 2, (COLS - 16) / 2, "Magic Wand: %d", backpack[2]);
        mvprintw(LINES / 2 + 2, (COLS - 16) / 2, "Normal Arrow: %d", backpack[3]);
        mvprintw(LINES / 2 + 4, (COLS - 16) / 2, "Sword: %d", backpack[4]);

        mvprintw(LINES / 2 - 4, (COLS - 22) / 2, "\U00002692");
        mvprintw(LINES / 2 - 2, (COLS - 22) / 2, "\U0001F5E1");
        mvprintw(LINES / 2, (COLS - 22) / 2, "\U0001FA84");
        mvprintw(LINES / 2 + 2, (COLS - 22) / 2, "\U000027B3");
        mvprintw(LINES / 2 + 4, (COLS - 22) / 2, "\U00002694");

        attron(A_REVERSE);
        mvprintw(LINES / 2 + 6, (COLS - 4) / 2, "Back");
        attroff(A_REVERSE);
        
        int c = getch();
        if(c == 10){
            break;
        }
    }

    clear();
    refresh();
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            attron(COLOR_PAIR(map[y][x].color_pair));
            mvprintw(y, x, "%c", map[y][x].ch);
            attroff(COLOR_PAIR(map[y][x].color_pair));
        }
    }
    refresh();
}

void spell_list(){
    character map[LINES][COLS];
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            map[y][x].ch = (mvinch(y, x) & A_CHARTEXT);
            map[y][x].color_pair = PAIR_NUMBER(mvinch(y, x) & A_COLOR);
        }
    }

    while(1){
        clear();
        attroff(A_REVERSE);
        attron(A_BOLD);
        mvprintw(LINES / 2 - 4, (COLS - 7) / 2, "Spells: ");
        attroff(A_BOLD);
        
        attron(COLOR_PAIR(10));
        mvprintw(LINES / 2 - 2, (COLS - 16) / 2, "Health: %d", spells[0]);

        attron(COLOR_PAIR(18));
        mvprintw(LINES / 2, (COLS - 16) / 2, "Speed: %d", spells[1]);

        attron(COLOR_PAIR(1));
        mvprintw(LINES / 2 + 2, (COLS - 16) / 2, "Damage: %d", spells[2]);
        attroff(COLOR_PAIR(1));

        attron(A_REVERSE);
        mvprintw(LINES / 2 + 4, (COLS - 4) / 2, "Back");
        attroff(A_REVERSE);
        
        int c = getch();
        if(c == 10){
            break;
        }
    }

    clear();
    refresh();
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            attron(COLOR_PAIR(map[y][x].color_pair));
            mvprintw(y, x, "%c", map[y][x].ch);
            attroff(COLOR_PAIR(map[y][x].color_pair));
        }
    }
    refresh();
}


int reverse(int password){
    int reversed = 0;
    for(int i = 0; i < 4; i++){
        reversed = reversed * 10 + password % 10;
        password /= 10;
    }
    return reversed;
}

void print_password_doors(){
    for(int i = 0; i < password_doors_count; i++){
        attron(COLOR_PAIR(9));
        if(password_doors[i].unlocked == 1){
            attron(COLOR_PAIR(10));
        }
        else{
            attron(COLOR_PAIR(1));
        }
        if((rooms[find_room(password_doors[i].x, password_doors[i].y)].explored == 0)){
            attron(COLOR_PAIR(20));
        }
        if((abs(player.x - password_doors[i].x) <= 1) && (abs(player.y - password_doors[i].y) <= 1)){
            attroff(COLOR_PAIR(20));
            if(password_doors[i].unlocked == 0){
                attron(COLOR_PAIR(1));
            }
            else{
                attron(COLOR_PAIR(10));
            }
        }
        mvprintw(password_doors[i].y, password_doors[i].x, "@");
        if(rooms[find_room(password_doors[i].x_button, password_doors[i].y_button)].explored == 1){
            attron(COLOR_PAIR(9));
            mvprintw(password_doors[i].y_button, password_doors[i].x_button, "&");
            attroff(COLOR_PAIR(9));
        }
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(10));
        attroff(COLOR_PAIR(20));
    }
}

void reveal_trap(struct point *traps, int trap_count, int x, int y){
    for(int i = 0; i < trap_count; i++){
        if(traps[i].x == x && traps[i].y == y){
            mvprintw(y, x, "^");
            return;
        }
    }
}

void print_map_with_colors(int floor_num){
    mvprintw(LINES - 1, 30, "Score:");
    mvprintw(LINES - 1, 50, "Hits: 0");
    mvprintw(LINES - 1, 70, "Str:");
    mvprintw(LINES - 1, 90, "Gold:");
    mvprintw(LINES - 1, 110, "Exp:");
    mvprintw(LINES - 1, 130, "Ancient Keys:");

    mvprintw(0, COLS - 25, "You are on Floor %d", floor_num);
    if(floor_num == 4){
        mvprintw(1, COLS - 30, "Press q to Finish the Game.");
    }
    mvprintw(LINES - 1, 37, "%d", gold);
    mvprintw(LINES - 1, 96, "%d", gold);
    mvprintw(LINES - 1, 75, "  ");
    mvprintw(LINES - 1, 75, "%d", strength);
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].color_pair != 0){
                attron(COLOR_PAIR(map[y][x].color_pair));
                mvprintw(y, x, "%c", map[y][x].ch);
                attroff(COLOR_PAIR(map[y][x].color_pair));
            }
            if(map[y][x].ch == '=' && rooms[find_room(x, y)].explored == 1){
                attroff(COLOR_PAIR(20));
                mvprintw(y, x, "=");
            }
        }
    }
    print_rooms();
    if(rooms[find_room(staircases[floor_num].x, staircases[floor_num].y)].explored == 1){
        mvprintw(staircases[floor_num].y, staircases[floor_num].x, "<");
    }
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].ch != '_' && map[y][x].ch != '|' && map[y][x].ch != '.'){
                if(rooms[find_room(x, y)].explored == 1){
                    attron(COLOR_PAIR(map[y][x].color_pair));
                    if(map[y][x].ch == 'O'){
                        attron(COLOR_PAIR(21));
                    }
                    if(map[y][x].ch == '*'){
                        attron(COLOR_PAIR(7));
                    }
                    if(map[y][x].ch == 'x'){
                        attron(COLOR_PAIR(15));
                    }
                    if(map[y][x].ch == 'M' || map[y][x].ch == 'D' || map[y][x].ch == 'W' || map[y][x].ch == 'A' || map[y][x].ch == 'S'){
                        attron(COLOR_PAIR(16));
                    }
                    if(map[y][x].ch == 'f' && map[y][x].color_pair == 20 && !map[y][x].color_check){
                        int random = rand() % 6;
                        if(random < 3){
                            map[y][x].color_pair = 14;
                        }
                        else if(random == 3){
                            map[y][x].color_pair = 7;
                        }
                        else if(random == 4){
                            map[y][x].color_pair = 13;
                        }
                        else if(random == 5){
                            map[y][x].color_pair = 10;
                        }
                        map[y][x].color_check = 1;
                        attron(COLOR_PAIR(map[y][x].color_pair));
                    }
                    if(map[y][x].ch == '$' && map[y][x].color_pair == 20 && !map[y][x].color_check){
                        int random = rand() % 3;
                        if(random == 0){
                            map[y][x].color_pair = 10;
                        }
                        if(random == 1){
                            map[y][x].color_pair = 18;
                        }
                        if(random == 2){
                            map[y][x].color_pair = 1;
                        }
                        map[y][x].color_check = 1;
                        attron(COLOR_PAIR(map[y][x].color_pair));
                    }
                    if(map[y][x].ch == '='){
                        attron(COLOR_PAIR(21));
                    }
                }
                if(rooms[find_room(x, y)].explored == 1){
                    mvprintw(y, x, "%c", map[y][x].ch);
                }
                attroff(COLOR_PAIR(map[y][x].color_pair));
            }
        }
    }

    attroff(COLOR_PAIR(20));
    int hallway_check = 1;
    for(int y = player.y - 1; y >= player.y - 5 && y >= 0; y--){
        if((mvinch(y, player.x) & A_CHARTEXT) != '#'){
            hallway_check = 0;
            break;
        }
    }

    if(hallway_check){
        for(int y = player.y - 1; y >= player.y - 5 && y >= 0; y--){
            attron(COLOR_PAIR(21));
            mvprintw(y, player.x, "#");
            attroff(COLOR_PAIR(21));
            map[y][player.x].ch = '#';
            map[y][player.x].color_pair = 21;
        }
    }

    hallway_check = 1;
    for(int y = player.y + 1; y <= player.y + 5 && y < LINES; y++){
        if((mvinch(y, player.x) & A_CHARTEXT) != '#'){
            hallway_check = 0;
            break;
        }
    }

    if(hallway_check){
        for(int y = player.y + 1; y <= player.y + 5 && y < LINES; y++){
            attron(COLOR_PAIR(21));
            mvprintw(y, player.x, "#");
            attroff(COLOR_PAIR(21));
            map[y][player.x].ch = '#';
            map[y][player.x].color_pair = 21;
        }
    }

    hallway_check = 1;
    for(int x = player.x - 1; x >= player.x - 5 && x >= 0; x--){
        if((mvinch(player.y, x) & A_CHARTEXT) != '#'){
            hallway_check = 0;
            break;
        }
    }

    if(hallway_check){
        for(int x = player.x - 1; x >= player.x - 5 && x >= 0; x--){
            attron(COLOR_PAIR(21));
            mvprintw(player.y, x, "#");
            attroff(COLOR_PAIR(21));
            map[player.y][x].ch = '#';
            map[player.y][x].color_pair = 21;
        }
    }

    hallway_check = 1;
    for(int x = player.x + 1; x <= player.x + 5 && x < COLS; x++){
        if((mvinch(player.y, x) & A_CHARTEXT) != '#'){
            hallway_check = 0;
            break;
        }
    }

    if(hallway_check){
        for(int x = player.x + 1; x <= player.x + 5 && x < COLS; x++){
            attron(COLOR_PAIR(21));
            mvprintw(player.y, x, "#");
            attroff(COLOR_PAIR(21));
            map[player.y][x].ch = '#';
            map[player.y][x].color_pair = 21;
        }
    }

    for(int y = player.y - 1; y <= player.y + 1; y++){
        if(y < 0 || y >= LINES){
            continue;
        }
        for(int x = player.x - 1; x <= player.x + 1; x++){
            if(x < 0 || x >= COLS){
                continue;
            }
            if((mvinch(y, x) & A_CHARTEXT) == '#'){
                attron(COLOR_PAIR(21));
                mvprintw(y, x, "#");
                attroff(COLOR_PAIR(21));
                map[y][x].ch = '#';
                map[y][x].color_pair = 21;
            }
        }
    }

    if(map[player.y][player.x].ch == '+'){
        mvprintw(player.y, player.x, "+");
    }
        
    for(int i = 0; i < 50; i++){
        if(rooms[find_room(doors[i].x, doors[i].y)].explored == 0){
            attron(COLOR_PAIR(20));
        }
        mvprintw(doors[i].y, doors[i].x, "+");
        attroff(COLOR_PAIR(20));
    }
    print_password_doors();
    if(ancient_key.full == 1){
        attron(COLOR_PAIR(7));
        if(rooms[find_room(ancient_key.x, ancient_key.y)].explored == 0){
            attron(COLOR_PAIR(20));
        }
        mvprintw(ancient_key.y, ancient_key.x, "\U000025B3");
        attroff(COLOR_PAIR(7));
        attroff(COLOR_PAIR(20));
    }
    mvprintw(LINES - 1, 30, "Score:");
    mvprintw(LINES - 1, 50, "Hits: 0");
    mvprintw(LINES - 1, 70, "Str:");
    mvprintw(LINES - 1, 90, "Gold:");
    mvprintw(LINES - 1, 110, "Exp:");
    mvprintw(LINES - 1, 130, "Ancient Keys:");

    mvprintw(0, COLS - 25, "You are on Floor %d", floor_num);
    if(floor_num == 4){
        mvprintw(1, COLS - 30, "Press q to Finish the Game.");
    }
    mvprintw(LINES - 1, 37, "%d", gold);
    mvprintw(LINES - 1, 96, "%d", gold);
    mvprintw(LINES - 1, 75, "  ");
    mvprintw(LINES - 1, 75, "%d", strength);
    mvprintw(LINES - 1, 96, "%d", gold);
    mvprintw(LINES - 1, 75, "%d", strength);
    mvprintw(LINES - 1, 144, "%d (%d Broken)", ancient_key_count / 2, ancient_key_count % 2);
}

void print_full_map(int floor_num){
    mvprintw(LINES - 1, 30, "Score:");
    mvprintw(LINES - 1, 50, "Hits: 0");
    mvprintw(LINES - 1, 70, "Str:");
    mvprintw(LINES - 1, 90, "Gold:");
    mvprintw(LINES - 1, 110, "Exp:");
    mvprintw(LINES - 1, 130, "Ancient Keys:");

    mvprintw(0, COLS - 25, "You are on Floor %d", floor_num);
    if(floor_num == 4){
        mvprintw(1, COLS - 30, "Press q to Finish the Game.");
    }
    mvprintw(LINES - 1, 37, "%d", gold);
    mvprintw(LINES - 1, 96, "%d", gold);
    mvprintw(LINES - 1, 75, "  ");
    mvprintw(LINES - 1, 75, "%d", strength);

    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].color_pair != 0){
                attron(COLOR_PAIR(map[y][x].color_pair));
                mvprintw(y, x, "%c", map[y][x].ch);
                attroff(COLOR_PAIR(map[y][x].color_pair));
            }
        }
    }

    for(int i = 0; i < room_count; i++){
        int x_start = rooms[i].x_top_left;
        int y_start = rooms[i].y_top_left;
        int width = rooms[i].x_size;
        int height = rooms[i].y_size;

        mvprintw(y_start, x_start, "_");
        for(int j = 1; j <= width; j++){
            move(y_start, x_start + j);
            printw("_");
        }
        mvprintw(y_start, x_start + width + 1, "_");

        for(int j = 1; j <= height; j++){
            move(y_start + j, x_start);
            printw("|");
            for(int k = 1; k <= width; k++){
                mvprintw(y_start + j, x_start + k, ".");
            }
            move(y_start + j, x_start + width + 1);
            printw("|");
        }

        mvprintw(y_start + height + 1, x_start, "_");
        for(int j = 1; j <= width; j++){
            move(y_start + height + 1, x_start + j);
            printw("_");
        }
        mvprintw(y_start + height + 1, x_start + width + 1, "_");
    }

    attron(COLOR_PAIR(5));
    mvprintw(player.y, player.x, "\U0001FBC5");
    attroff(COLOR_PAIR(5));
    mvprintw(staircases[floor_num].y, staircases[floor_num].x, "<");

    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].ch != '_' && map[y][x].ch != '|' && map[y][x].ch != '.'){
                if(map[y][x].ch == '#'){
                    attron(COLOR_PAIR(21));
                }
                else if(map[y][x].ch == '+'){
                    attron(COLOR_PAIR(21));
                }
                else if(map[y][x].color_pair != 20){
                    attron(COLOR_PAIR(map[y][x].color_pair));
                }
                if(map[y][x].ch == '*'){
                    attron(COLOR_PAIR(7));
                }
                if(map[y][x].ch == 'x'){
                    attron(COLOR_PAIR(15));
                }
                if(map[y][x].ch == 'M' || map[y][x].ch == 'D' || map[y][x].ch == 'W' || map[y][x].ch == 'A' || map[y][x].ch == 'S'){
                    attron(COLOR_PAIR(16));
                }
                if(map[y][x].ch == 'f' && !map[y][x].color_check){
                    int random = rand() % 6;
                    if(random < 3){
                        map[y][x].color_pair = 14;
                    }
                    else if(random == 3){
                        map[y][x].color_pair = 7;
                    }
                    else if(random == 4){
                        map[y][x].color_pair = 13;
                    }
                    else if(random == 5){
                        map[y][x].color_pair = 10;
                    }
                    map[y][x].color_check = 1;
                    attron(COLOR_PAIR(map[y][x].color_pair));
                }
                if(map[y][x].ch == '$' && !map[y][x].color_check){
                    int random = rand() % 3;
                    if(random == 0){
                        map[y][x].color_pair = 10;
                    }
                    else if(random == 1){
                        map[y][x].color_pair = 18;
                    }
                    else if(random == 2){
                        map[y][x].color_pair = 1;
                    }
                    map[y][x].color_check = 1;
                    attron(COLOR_PAIR(map[y][x].color_pair));
                }
                if(map[y][x].ch == '='){
                    attron(COLOR_PAIR(21));
                }
                mvprintw(y, x, "%c", map[y][x].ch);
                if(map[y][x].ch != '#' && map[y][x].ch != '+'){
                    attroff(COLOR_PAIR(map[y][x].color_pair));
                }
            }
        }
    }

    init_pair(21, COLOR_WHITE, COLOR_BLACK);
    if(map[player.y][player.x].ch == '+'){
        attron(COLOR_PAIR(21));
        mvprintw(player.y, player.x, "+");
        
    }

    for(int i = 0; i < 50; i++){
        attron(COLOR_PAIR(21));
        mvprintw(doors[i].y, doors[i].x, "+");
    }
    print_password_doors();
}

void initialize_map(){
    map = malloc(LINES * sizeof(character *));
    if(map == NULL){
        fprintf(stderr, "Error allocating memory for map\n");
        exit(1);
    }
    for(int i = 0; i < LINES; i++){
        map[i] = malloc(COLS * sizeof(character));
        if(map[i] == NULL){
            fprintf(stderr, "Error allocating memory for map[%d]\n", i);
            exit(1);
        }
    }
}

void reveal_room_by_window(int x, int y){
    int wall_x, wall_y;
    if(map[y][x + 1].ch == '.'){
        wall_x = x;
        wall_y = y;
        while(wall_x < COLS - 1 && map[wall_y][wall_x].ch != '|'){
            wall_x++;
        }
        if(wall_x >= 0 && wall_x < COLS){
            int room = find_room(wall_x - 1, wall_y);
            if(room != -1){
                rooms[room].explored = 1;
            }
        }
    }
    if(map[y][x - 1].ch == '.'){
        wall_x = x;
        wall_y = y;
        while(wall_x > 0 && map[wall_y][wall_x].ch != '|'){
            wall_x--; 
        }
        if(wall_x >= 0 && wall_x < COLS){
            int room = find_room(wall_x + 1, wall_y);
            if(room != -1){
                rooms[room].explored = 1;
            }
        }
    }
    if(map[y + 1][x].ch == '.'){
        wall_x = x;
        wall_y = y;
        while(wall_y > 0 && map[wall_y][wall_x].ch != '_'){
            wall_y--;
        }
        if(wall_y >= 0 && wall_y < LINES){
            int room = find_room(wall_x, wall_y + 1);
            if(room != -1){
                rooms[room].explored = 1;
            }
        }
    }
    if(map[y - 1][x].ch == '.'){
        wall_x = x;
        wall_y = y;
        while(wall_y < LINES - 1 && map[wall_y][wall_x].ch != '_'){
            wall_y++;
        }
        if(wall_y >= 0 && wall_y < LINES){
            int room_index = find_room(wall_x, wall_y - 1);
            if(room_index != -1){
                rooms[room_index].explored = 1;
            }
        }
    }
}

int game_pause(){
    char *options[3] ={"Resume", "Save and Quit", "Quit"};
    int check = 0, highlight = 0;
    while(!check){
        clear();
        attroff(A_REVERSE);
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(LINES / 2 - 4, (COLS - 6) / 2, "Paused");
        attroff(A_BOLD | A_UNDERLINE);

        for(int i = 0; i < 3; i++){
            if(highlight == i){
                attron(A_REVERSE);
            }
            else{
                attroff(A_REVERSE);
            }
            mvprintw(LINES / 2 - 1 + 2 * i, (COLS - strlen(options[i])) / 2, "%s", options[i]);
        }

        int c = getch();
        if(c == KEY_DOWN){
            if(highlight == 2){
                highlight = 0;
            }
            else{
                highlight++;
            }
        }
        if(c == KEY_UP){
            if(highlight == 0){
                highlight = 2;
            }
            else{
                highlight--;
            }
        }
        if(c == 10){
            if(highlight == 0){
                return 0;
            }
            else if(highlight == 1){
                return 1;
            }
            else if(highlight == 2){
                return 2;
            }
        }
    }
}

int enter_floor(char *username, char color, char difficulty, int floor_num, char *track_name){
    clear();
    start_time = time(NULL);
    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", username);
    FILE *map_file = fopen(filename, "r");

    char line[COLS + 1];
    int check = 0;
    clear();
    move(0, 0);
    printw("\n");

    init_pair(21, COLOR_WHITE, COLOR_BLACK);
    
    while(fgets(line, sizeof(line), map_file) != NULL){
        char map_str[20];
        snprintf(map_str, sizeof(map_str), "Map%d:", floor_num);
        char format_str[20];
        snprintf(format_str, sizeof(format_str), "Room Count%d: %%d", floor_num);
        if(strstr(line, map_str) != NULL){
            check = 1;
            continue;
        }
        if(sscanf(line, format_str, &room_count) == 1){
            continue;
        } 
        if(check){
            int len = strlen(line);
            if(len > 0 && line[len - 1] == '\n'){
                line[len - 1] = '\0';
            }
            printw("%s", line);
        }
    }

    if(difficulty == 'e'){
        d = 1;
    }
    else if(difficulty == 'm'){
        d = 2;
    }
    else if(difficulty == 'h'){
        d = 3;
    }

    init_pair(20, COLOR_BLACK, COLOR_BLACK);
    initialize_map();
    
    int door_num = 0;
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            map[y][x].ch = (mvinch(y, x) & A_CHARTEXT);
            map[y][x].color_pair = PAIR_NUMBER(mvinch(y, x) & A_COLOR);
            if(map[y][x].ch == '+'){
                doors[door_num].x = x;
                doors[door_num].y = y;
                door_num++;
            }
        }
    }

    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].ch == '+' || map[y][x].ch == '='){
                if((x > 0 && (map[y][x - 1].ch == '.' || map[y][x - 1].ch == 'O')) || 
                    (x < COLS - 1 && (map[y][x + 1].ch == '.' || map[y][x + 1].ch == 'O'))){
                    mvprintw(y, x, "|");
                }
                else if((y > 0 && (map[y - 1][x].ch == '.' || map[y - 1][x].ch == 'O')) || (y < LINES - 1 && (map[y + 1][x].ch == '.' || map[y + 1][x].ch == 'O'))){
                    mvprintw(y, x, "_");
                }
            }
        }
    }

    attroff(COLOR_PAIR(20));
    fclose(map_file);
    
    int room_num = 0;
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(is_top_left(x, y) && room_num <= room_count){
                rooms[room_num].x_top_left = x;
                rooms[room_num].y_top_left = y;
                if((mvinch(y, x) & A_CHARTEXT) == 'U'){
                    rooms[room_num].explored = 0;
                }
                else{
                    rooms[room_num].explored = 1;
                }
                mvprintw(y, x, "_");
                map[y][x].ch = '_';

                int i = 1;
                rooms[room_num].x_size = 0;
                while(mvinch(y, x + i) == '_'){
                    rooms[room_num].x_size++;
                    i++;
                }
                rooms[room_num].x_size--;

                i = 1;
                rooms[room_num].y_size = 0;
                while(mvinch(y + i, x) == '|'){
                    rooms[room_num].y_size++;
                    i++;
                }

                room_num++;
            }
            
        }
    }

    clear();
    print_rooms();
    
    if(color == 'w'){
        init_pair(5, COLOR_WHITE, COLOR_BLACK);
    }
    if(color == 'r'){
        init_pair(5, COLOR_RED, COLOR_BLACK);
    }
    if(color == 'b'){
        init_pair(5, COLOR_BLUE, COLOR_BLACK);
    }
    if(color == 'g'){
        init_pair(5, COLOR_GREEN, COLOR_BLACK);
    }
    if(color == 'y'){
        init_color(COLOR_YELLOW, 1000, 1000, 0);
        init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    }

    if(player.x == -1 && player.y == -1){
        char character;
        do{
            player.x = random_with_range(0, COLS - 1);
            player.y = random_with_range(0, LINES - 1);
            chtype ch = mvinch(player.y, player.x);
            character = ch & A_CHARTEXT;
        } while(character != '.');
        player.under.ch = '.';
    }

    attron(COLOR_PAIR(5));
    mvprintw(player.y, player.x, "\U0001FBC5");
    attroff(COLOR_PAIR(5));
    mvprintw(0, 0, "%d",find_room(player.x, player.y));

    rooms[find_room(player.x, player.y)].theme = 1;
    rooms[find_room(player.x, player.y)].explored = 1;

    generate_staircase(floor_num);

    for(int i = 0; i < room_count; i++){
        int random_theme = rand();
        if(rooms[i].explored == 0){
            if(random_theme % 10 < 5){
                rooms[i].theme = 1;
            }
            else if(random_theme % 10 < 8){
                rooms[i].theme = 2;
            }
            else{
                rooms[i].theme = 3;
            }
        }
        else{
            rooms[i].theme = 1;
        }
    }

    print_rooms();

    attron(COLOR_PAIR(20));
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(map[y][x].ch != '_' && map[y][x].ch != '|' && map[y][x].ch != '.'){
                mvprintw(y, x, "%c", map[y][x].ch);
            }
        }
    }
    attroff(COLOR_PAIR(20));
    generate_gold();
    generate_weapon();
    generate_pillars();
    generate_food();
    generate_spell();
    generate_ancient_key();
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            map[y][x].ch = (mvinch(y, x) & A_CHARTEXT);
            map[y][x].color_pair = PAIR_NUMBER(mvinch(y, x) & A_COLOR);
            
        }
    }

    struct point traps[10];
    int trap_count;
    if(difficulty == 'e'){
        trap_count = rand_with_range(1, 3);
    }
    if(difficulty == 'm'){
        trap_count = rand_with_range(4, 6);
    }
    if(difficulty == 'h'){
        trap_count = rand_with_range(6, 10);
    }
    int i = 0;
    int x, y;
    while(i < trap_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '.' && !is_neighbours_with('+', x, y)){
            traps[i].y = y;
            traps[i].x = x;
            i++;
        }
    }

    i = 0;
    while(i < password_doors_count){
        x = random_with_range(1, COLS);
        y = random_with_range(1, LINES);
        if((mvinch(y, x) & A_CHARTEXT) == '+' && !is_neighbours_with('@', x, y)){
            attron(COLOR_PAIR(8));
            mvprintw(y, x, "@");
            password_doors[i].y = y;
            password_doors[i].x = x;
            password_doors[i].unlocked = 0;
            password_doors[i].password = -1;
            
            if((mvinch(y + 1, x + 1) & A_CHARTEXT) == '.'){
                password_doors[i].x_button = x + 1;
                password_doors[i].y_button = y + 1;
            }
            else if((mvinch(y + 1, x - 1) & A_CHARTEXT) == '.'){
                password_doors[i].x_button = x - 1;
                password_doors[i].y_button = y + 1;
            }
            else if((mvinch(y - 1, x + 1) & A_CHARTEXT) == '.'){
                password_doors[i].x_button = x + 1;
                password_doors[i].y_button = y - 1;
            }
            else if((mvinch(y - 1, x - 1) & A_CHARTEXT) == '.'){
                password_doors[i].x_button = x - 1;
                password_doors[i].y_button = y - 1;
            }
            else{
                password_doors[i].x_button = x; 
                password_doors[i].y_button = y;
            }

            attron(COLOR_PAIR(9));
            mvprintw(password_doors[i].y_button, password_doors[i].x_button, "&");
            attroff(COLOR_PAIR(9));
            i++;
        }
    }

    attroff(COLOR_PAIR(8));

    char previous_under = ' ';
    
    long int current_time, hunger_full_start_time;
    int hunger_full = 0;
    start_time = time(NULL);
    
    print_map_with_colors(floor_num);
    int current_room, prev_room = -1;
    while(1){
        current_time = time(NULL);
        
        if(difftime(current_time, start_time) >= 25 - 5 * d && hunger < 20){
            hunger++;
            start_time = current_time;
        }

        if(hunger == 20 && hunger_full == 0){
            hunger_full = 1;
            hunger_full_start_time = time(NULL);
        }
        if(hunger < 20){
            hunger_full = 0;
        }

        if(difftime(current_time, hunger_full_start_time) >= 10 && hunger_full){
            if(strength > 0){
                strength--;
            }
            hunger_full_start_time = current_time;
        }
        print_password_doors();
        mvprintw(LINES - 1, 30, "Score:");
        mvprintw(LINES - 1, 50, "Hits: 0");
        mvprintw(LINES - 1, 70, "Str:");
        mvprintw(LINES - 1, 90, "Gold:");
        mvprintw(LINES - 1, 110, "Exp:");
        mvprintw(LINES - 1, 130, "Ancient Keys:");

        mvprintw(0, COLS - 25, "You are on Floor %d", floor_num);
        if(floor_num == 4){
            mvprintw(1, COLS - 30, "Press q to Finish the Game.");
        }
        mvprintw(LINES - 1, 37, "%d", gold);
        mvprintw(LINES - 1, 96, "%d", gold);
        mvprintw(LINES - 1, 75, "  ");
        mvprintw(LINES - 1, 75, "%d", strength);

        attron(COLOR_PAIR(5));
        mvprintw(player.y, player.x, "\U0001FBC5");
        attroff(COLOR_PAIR(5));
        refresh();

        mvprintw(LINES - 1, 96, "%d", gold);
        mvprintw(LINES - 1, 75, "%d", strength);
        mvprintw(LINES - 1, 144, "%d (%d Broken)", ancient_key_count / 2, ancient_key_count % 2);

        attron(COLOR_PAIR(5));
        mvprintw(player.y, player.x, "\U0001FBC5");
        attroff(COLOR_PAIR(5));

        refresh();
        
        int c = getch();
        print_map_with_colors(floor_num);

        int f_check = 0;
        if(c == 'f' || c == 'f'){
            f_check = 1;
            c = getch();
        }
        int g_check = 0;
        if(c == 'g' || c == 'G'){
            g_check = 1;
            c = getch();
        }
        
        if(c == 'm' || c == 'M'){
            print_full_map(floor_num);
        }

        if(c == 's' || c == 'S'){
            for(int delta_x = -1; delta_x <= 1; delta_x++){
                for(int delta_y = -1; delta_y <= 1; delta_y++){
                    if(delta_x == 0 && delta_y == 0){
                        continue;
                    }

                    reveal_trap(traps, trap_count, player.x + delta_x, player.y + delta_y);
                }
            }
            c = getch();
        }
        for(int i = 0; i < 50; i++){
            if(rooms[find_room(doors[i].x, doors[i].y)].explored == 0){
                attron(COLOR_PAIR(20));
            }
            mvprintw(doors[i].y, doors[i].x, "+");
            attroff(COLOR_PAIR(20));
        }
        print_password_doors();
        attron(COLOR_PAIR(player.under.color_pair));
        if(player.under.ch == '.'){
            attroff(COLOR_PAIR(player.under.color_pair));
        }
        mvprintw(player.y, player.x, "%c", player.under.ch);
        attroff(COLOR_PAIR(player.under.color_pair));

        int previous_x = player.x, previous_y = player.y;

        if((c == 'h' || c == 'H') && check_movement(floor_num, player.x - 1, player.y)){
            if(f_check == 0){
                player.x--;
            }
            else{
                while(check_movement(floor_num, player.x - 1, player.y)){
                    player.x--;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'j' || c == 'J') && check_movement(floor_num, player.x, player.y + 1)){
            if(f_check == 0){
                player.y++;
            }
            else{
                while(check_movement(floor_num, player.x, player.y + 1)){
                    player.y++;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'k' || c == 'K') && check_movement(floor_num, player.x, player.y - 1)){
            if(f_check == 0){
                player.y--;
            }
            else{
                while(check_movement(floor_num, player.x, player.y - 1)){
                    player.y--;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'l' || c == 'L') && check_movement(floor_num, player.x + 1, player.y)){
            if(f_check == 0){
                player.x++;
            }
            else{
                while(check_movement(floor_num, player.x + 1, player.y)){
                    player.x++;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'y' || c == 'Y') && check_movement(floor_num, player.x - 1, player.y - 1)){
            if(f_check == 0){
                player.x--;
                player.y--;
            }
            else{
                while(check_movement(floor_num, player.x - 1, player.y - 1)){
                    player.x--;
                    player.y--;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'u' || c == 'U') && check_movement(floor_num, player.x + 1, player.y - 1)){
            if(f_check == 0){
                player.x++;
                player.y--;
            }
            else{
                while(check_movement(floor_num, player.x + 1, player.y - 1)){
                    player.x++;
                    player.y--;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'b' || c == 'B') && check_movement(floor_num, player.x - 1, player.y + 1)){
            if(f_check == 0){
                player.x--;
                player.y++;
            }
            else{
                while(check_movement(floor_num, player.x - 1, player.y + 1)){
                    player.x--;
                    player.y++;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if((c == 'n' || c == 'N') && check_movement(floor_num, player.x + 1, player.y + 1)){
            if(f_check == 0){
                player.x++;
                player.y++;
            }
            else{
                while(check_movement(floor_num, player.x + 1, player.y + 1)){
                    player.x++;
                    player.y++;
                    if(find_room(y, x) != -1){
                        if(rooms[find_room(y, x)].theme != 3){
                            rooms[find_room(y, x)].explored = 1;
                        }
                    }
                    if(map[player.y][player.x].ch == '#'){
                        attron(COLOR_PAIR(21));
                        mvprintw(player.y, player.x, "#");
                        map[player.y][player.x].color_pair = 21;
                    }
                }
            }
        }
        else if(c == 'e' || c == 'E'){
            food_list(&strength);
        }
        else if(c == 'i' || c == 'I'){
            weapon_list();
        }
        else if(c == 'x' || c == 'X'){
            spell_list();
        }
        else if(c == 'q' && floor_num == 4){
            return -1;
        }
        else if(c == 27){
            int choice = game_pause();
            if(choice == 1){
                return -1 * floor_num;
            }
            if(choice == 2){
                return -5;
            }
        }
        
        for(int y = 0; y < LINES; y++){
            for(int x = 0; x < COLS; x++){
                if(map[y][x].ch == '=' && find_room(x, y) == find_room(player.x, player.y)){
                    reveal_room_by_window(x, y);
                    
                }
            }
        }

        player.under.ch = mvinch(player.y, player.x) & A_CHARTEXT;
        player.under.color_pair = PAIR_NUMBER(mvinch(player.y, player.x) & A_COLOR); 

        if(player.under.ch == '&' && previous_under != '&'){
            attron(COLOR_PAIR(5));
            mvprintw(player.y, player.x, "\U0001FBC5");
            attroff(COLOR_PAIR(5));
            int password_door_num = find_door_to_button(player.x, player.y);
            password_doors[password_door_num].password = rand_with_range(1000, 9999);
            getch();
            print_map_with_colors(floor_num);
            
            display_door_password(password_doors[password_door_num].password, floor_num);
            if((rand() % 5 == 0) && password_doors[password_door_num].password % 10 != 0){
                password_doors[password_door_num].password = reverse(password_doors[password_door_num].password);
            }
        }
        
        previous_under = player.under.ch;

        if(player.under.ch == '*' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            gold++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found Gold.");
            getch();
            mvprintw(0, 0, "               ");
        }
        if(player.under.ch == 'x' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            gold += 10;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found Black Gold.");
            getch();
            mvprintw(0, 0, "                     ");
        }
        if(player.under.ch == 'f' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found Food.");
            getch();
            mvprintw(0, 0, "               ");
        }
        if(player.under.ch == 'M' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            backpack[0]++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Mace.");
            getch();
            mvprintw(0, 0, "                 ");
        }
        if(player.under.ch == 'D' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            backpack[1]++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Dagger.");
            getch();
            mvprintw(0, 0, "                   ");
        }
        if(player.under.ch == 'W' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            backpack[2]++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Magic Wand.");
            getch();
            mvprintw(0, 0, "                       ");
        }
        if(player.under.ch == 'A' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            backpack[3]++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Normal Arrow.");
            getch();
            mvprintw(0, 0, "                         ");
        }
        if(player.under.ch == 'S' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            backpack[4]++;
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Sword.");
            getch();
            mvprintw(0, 0, "                  ");
        }
        if(player.under.ch == '$' && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            map[player.y][player.x].ch = '.';
            print_map_with_colors(floor_num);
            mvprintw(0, 0, "You Found a Spell.");
            getch();
            mvprintw(0, 0, "                  ");
        }
        if(player.under.ch == '<'){
            attron(COLOR_PAIR(5));
            mvprintw(player.y, player.x, "\U0001FBC5");
            attroff(COLOR_PAIR(5));
            char cmd = getch();
            if(cmd == '>'){
                if(floor_num == 1){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "You Are Already on the Lowest Floor.");
                    attroff(COLOR_PAIR(1));
                    getch();
                    mvprintw(0, 0, "                                    ");
                }
                else{
                    return (floor_num - 1);
                }
            }
            if(cmd == '<'){
                if(floor_num == 4){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "You Are Already on the Highest Floor.");
                    attroff(COLOR_PAIR(1));
                    getch();
                    mvprintw(0, 0, "                                     ");
                }
                else{
                    return (floor_num + 1);
                }
            }
        }

        if(ancient_key.x == player.x && ancient_key.y == player.y && ancient_key.full == 1 && g_check == 0 && rooms[find_room(player.x, player.y)].theme != 3){
            player.under.ch = '.';
            ancient_key.full = 0;
            ancient_key_count += 2;
        }

        attron(COLOR_PAIR(5));
        mvprintw(player.y, player.x, "\U0001FBC5");
        attroff(COLOR_PAIR(5));
        
        int trap_num = check_trap(player.x, player.y, traps, trap_count, &strength);
        if(trap_num != -1){
            player.under.ch = '^';
            map[player.y][player.x].color_pair = 21;
            map[player.y][player.x].ch = '^';
        }
        
        current_room = find_room(player.x, player.y);
        if(current_room != -1){
            if(rooms[current_room].explored == 0){
                mvprintw(0, 0, "You Are Exploring a New Room.");
                if(rooms[current_room].theme != 3){
                    rooms[current_room].explored = 1;
                }
            }
            if(strcmp(track_name, "off") != 0){
                if(current_room != prev_room){
                    if(rooms[current_room].theme == 2){
                        play_music("enchant.mp3");
                    }
                    else if(rooms[current_room].theme == 3){
                        play_music("nightmare.mp3");
                    }
                }
            }
        } 
        else if(player.under.ch == '#' && prev_room != -1 && rooms[prev_room].theme != 1 && strcmp(track_name, "off") != 0){
            play_music(track_name);
        }
        prev_room = current_room;
        if(rooms[current_room].theme == 3){
            for(int delta_x = -1; delta_x <= 1; delta_x++){
                for(int delta_y = -1; delta_y <= 1; delta_y++){
                    int x = player.x + delta_x;
                    int y = player.y + delta_y;
                    if(map[y][x].color_pair != 0){
                        attron(COLOR_PAIR(21));
                        if(map[y][x].ch == '*'){
                            attron(COLOR_PAIR(7));
                        }
                        if(map[y][x].ch == 'x'){
                            attron(COLOR_PAIR(15));
                        }
                        if(map[y][x].ch == 'M' || map[y][x].ch == 'D' || map[y][x].ch == 'W' || map[y][x].ch == 'A' || map[y][x].ch == 'S'){
                            attron(COLOR_PAIR(16));
                        }
                        if(map[y][x].ch == 'f'){
                            if(!map[y][x].color_check){
                                int random = rand() % 6;
                                if(random < 3){
                                    map[y][x].color_pair = 14;
                                }
                                else if(random == 3){
                                    map[y][x].color_pair = 7;
                                }
                                else if(random == 4){
                                    map[y][x].color_pair = 13;
                                }
                                else if(random == 5){
                                    map[y][x].color_pair = 10;
                                }
                                map[y][x].color_check = 1;
                            }
                            attron(COLOR_PAIR(map[y][x].color_pair));
                        }   
                        if(map[y][x].ch == '$' && !map[y][x].color_check){
                            int random = rand() % 3;
                            if(random == 0){
                                map[y][x].color_pair = 10;
                            }
                            else if(random == 1){
                                map[y][x].color_pair = 18;
                            }
                            else if(random == 2){
                                map[y][x].color_pair = 1;
                            }
                            map[y][x].color_check = 1;
                            attron(COLOR_PAIR(map[y][x].color_pair));
                        }
                        mvprintw(y, x, "%c", map[y][x].ch);
                        attroff(COLOR_PAIR(map[y][x].color_pair));
                    }
                }
            }
        }
        if(strength == 0){
            return -6;
        }
    }

    clear();
    endwin();
}

void save_game(char* filename, int floor_num){
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            mvprintw(y, x, "%c", map[y][x].ch);
        }
    }
    for(int i = 0; i < room_count; i++){
        if(rooms[i].explored == 1){
            mvprintw(rooms[i].y_top_left, rooms[i].x_top_left, "E");
        }
        else{
            mvprintw(rooms[i].y_top_left, rooms[i].x_top_left, "U");
        }
    }
    getch();

    FILE *file = fopen(filename, "r+");

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *file_content = malloc(file_size + 1);
    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';

    int previous_score, previous_gold, games_played;
    long int first_time;
    sscanf(file_content, "Time: %ld\nScore: %d\nGold: %d\nGames Played: %d\n", &first_time, &previous_score, &previous_gold, &games_played);
    games_played += 1;

    char new_text[1000];
    snprintf(new_text, sizeof(new_text), "Time: %ld\nScore: %d\nGold: %d\nGames Played: %d\nStrength: %d\nHunger: %d\nNormal Food: %d\nDeluxe Food: %d\nMagical Food: %d\nRotten Food %d\nMace: %d\nDagger: %d\nWand: %d\nArrow: %d\nSword: %d\nHealth Spell: %d\nSpeed Spell: %d\nDamage Spell: %d\nAncient Key: %d\nFloor: %d\nPlayer y: %d\nPlayer x: %d\n", start_time, gold, gold, games_played, strength, hunger, food[0], food[1], food[2], food[3], backpack[0], backpack[1], backpack[2], backpack[3], backpack[4], spells[0], spells[1], spells[2], ancient_key_count, floor_num, player.y, player.x);
    char *room_count_position = strstr(file_content, "Room Count1: ");

    if(room_count_position){
        long offset = room_count_position - file_content;
        snprintf(file_content, offset + 1, "%s", new_text);
        strcat(file_content, room_count_position);
    }
    else{
        snprintf(file_content, file_size + 1, "%s", new_text);
    }

    freopen(NULL, "w", file);
    fputs(file_content, file);

    free(file_content);
    fclose(file);
}

void play(char *username, char color, char difficulty, int song){
    int floor_num;

    char filename[100];
    strcpy(filename, username);
    strcat(filename, ".txt");

    FILE *file = fopen(filename, "r");

    char line[COLS - 1];
    int i = 0;
    while(fgets(line, sizeof(line), file)){
        if(sscanf(line, "Gold: %d", &gold) == 1){
            continue;
        }
        if(sscanf(line, "Strength: %d", &strength) == 1){
            continue;
        }
        if(sscanf(line, "Hunger: %d", &hunger) == 1){
            continue;
        }
        if(sscanf(line, "Normal Food: %d", &food[0]) == 1){
            continue;
        }
        if(sscanf(line, "Deluxe Food: %d", &food[1]) == 1){
            continue;
        }
        if(sscanf(line, "Magical Food: %d", &food[2]) == 1){
            continue;
        }
        if(sscanf(line, "Rotten Food: %d", &food[3]) == 1){
            continue;
        }
        if(sscanf(line, "Mace: %d", &backpack[0]) == 1){
            continue;
        }
        if(sscanf(line, "Dagger: %d", &backpack[1]) == 1){
            continue;
        }
        if(sscanf(line, "Wand: %d", &backpack[2]) == 1){
            continue;
        }
        if(sscanf(line, "Arrow: %d", &backpack[3]) == 1){
            continue;
        }
        if(sscanf(line, "Sword: %d", &backpack[4]) == 1){
            continue;
        }
        if(sscanf(line, "Health Spell: %d", &spells[0]) == 1){
            continue;
        }
        if(sscanf(line, "Speed Spell: %d", &spells[1]) == 1){
            continue;
        }
        if(sscanf(line, "Damage Spell: %d", &spells[2]) == 1){
            continue;
        }
        if(sscanf(line, "Ancient Key: %d", &ancient_key_count) == 1){
            continue;
        }
        if(sscanf(line, "Floor: %d", &floor_num) == 1){
            continue;
        }
        if(sscanf(line, "Player y: %d", &player.y) == 1){
            continue;
        }
        if(sscanf(line, "Player x: %d", &player.x) == 1){
            continue;
        }
    }

    fclose(file);
    char track_name[15];

    if (song != 0) {
        snprintf(track_name, sizeof(track_name), "track_%d.mp3", song);
    } else {
        snprintf(track_name, sizeof(track_name), "off");
    }

    start_check = 1;
    while (floor_num >= 0) {
        floor_num = enter_floor(username, color, difficulty, floor_num, track_name);
        start_check = 0;
        clear();
    }

    if(floor_num == -4){
        attron(A_BOLD);
        mvprintw(LINES / 2 - 2, (COLS - 7) / 2, "You Win");
        attroff(A_BOLD);
        mvprintw(LINES / 2, (COLS - 8) / 2, "Score: %d", gold);
        mvprintw(LINES / 2 + 2, (COLS - 28) / 2, "Press Any Key to Continue...");
        getch();
    }
    if(floor_num == -6){
        attron(A_BOLD);
        mvprintw(LINES / 2 - 2, (COLS - 7) / 2, "You Lose");
        attroff(A_BOLD);
        mvprintw(LINES / 2, (COLS - 8) / 2, "Score: %d", gold);
        mvprintw(LINES / 2 + 2, (COLS - 28) / 2, "Press Any Key to Continue...");
        getch();
    }

    if(floor_num > -5 || floor_num == -6){
        save_game(filename, -1 * floor_num);
    }

    clear();
}