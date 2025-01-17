#ifndef GENERATE_MAP_H
#define GENERATE_MAP_H

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

struct room{
    int x_top_left;
    int y_top_left;
    int x_size;
    int y_size;
    int theme;
    int explored;
};

struct hallway{
    int x_1;
    int y_1;
    int x_2;
    int y_2;
};

int rand_with_range(int min_rand, int max_rand){
    return ((rand() % (max_rand - min_rand + 1)) + min_rand);
}

int is_neighbours_with(char c, int x, int y){
    for(int delta_x = -1; delta_x <= 1; delta_x++){
        for(int delta_y = -1; delta_y <= 1; delta_y++){
            if(delta_x == 0 && delta_y == 0){
                continue;
            }
            if(mvinch(y + delta_y, x + delta_x) == c){
                return 1;
            }
        }
    }
    return 0;
}

void print_room(struct room *rooms, int room_count){
    int x_start = rooms[room_count].x_top_left;
    int y_start = rooms[room_count].y_top_left;
    int width = rooms[room_count].x_size;
    int height = rooms[room_count].y_size;

    mvprintw(y_start, x_start, "_");
    for(int i = 1; i <= width; i++){
        move(y_start, x_start + i);
        printw("_");
    }
    mvprintw(y_start, x_start + width + 1, "_");

    for(int i = 1; i <= height; i++){
        move(y_start + i, x_start);
        printw("|");
        for(int j = 1; j <= width; j++){
            mvprintw(y_start + i, x_start + j, ".");
        }
        move(y_start + i, x_start + width + 1);
        printw("|");
    }

    mvprintw(y_start + height + 1, x_start, "_");
    for(int i = 1; i <= width; i++){
        move(y_start + height + 1, x_start + i);
        printw("_");
    }
    mvprintw(y_start + height + 1, x_start + width + 1, "_");
}

int check_room(struct room *rooms, int room_count){
    int x_start = rooms[room_count].x_top_left;
    int y_start = rooms[room_count].y_top_left;
    int width = rooms[room_count].x_size;
    int height = rooms[room_count].y_size;

    for(int i = 0; i < room_count; i++){
        int x_start_i = rooms[i].x_top_left;
        int y_start_i = rooms[i].y_top_left;
        int width_i = rooms[i].x_size;
        int height_i = rooms[i].y_size;

        if((x_start < x_start_i + width_i + 10) && (x_start + width + 10 > x_start_i) && (y_start < y_start_i + height_i + 10) && (y_start + height + 10 > y_start_i)){
            return 0;
        }
    }
    return 1;
}

void print_hallway(struct hallway *hallways, int hallway_count){
    int x = hallways[hallway_count].x_1;
    int y = hallways[hallway_count].y_1;
    int x2 = hallways[hallway_count].x_2;
    int y2 = hallways[hallway_count].y_2;

    while(x != x2){
        if(x < x2){
            x++;
        }
        else{
            x--;
        }

        if(mvinch(y, x) == '|' || mvinch(y, x) == '_'){
            mvprintw(y, x, "+");
        }
        else if(mvinch(y, x) != '.' && mvinch(y, x) != '+'){
            mvprintw(y, x, "#");
        }
    }

    while(y != y2){
        if(y < y2){
            y++;
        }
        else{
            y--;
        }

        if(mvinch(y, x) == '|' || mvinch(y, x) == '_'){
            mvprintw(y, x, "+");
        }
        else if(mvinch(y, x) != '.' && mvinch(y, x) != '+'){
            mvprintw(y, x, "#");
        }
    }

    if(mvinch(y2, x2) == '|' || mvinch(y2, x2) == '_'){
        mvprintw(y2, x2, "+");
    }
    else if(mvinch(y2, x2) != '.' && mvinch(y2, x2) != '+'){
        mvprintw(y2, x2, "#");
    }
}

void generate_pillars(){
    int pillar_count = rand_with_range(5, 10);
    int i = 0;
    int x, y;
    while(i < pillar_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if(mvinch(y, x) == '.' && !is_neighbours_with('+', x, y) && !is_neighbours_with('=', x, y)){
            mvprintw(y, x, "O");
            i++;
        }
    }
}

int corner(int x, int y){
    if(mvinch(y, x) == '|'){
        if(mvinch(y - 1, x) == '_' || mvinch(y + 1, x) == '_'){
            return 1;
        }
    }

    if(mvinch(y, x) == '_'){
        if(mvinch(y - 1, x) == '|' || mvinch(y + 1, x) == '|'){
            return 1;
        }
    }

    return 0;
}

void generate_windows(){
    int window_count = rand_with_range(5, 10);
    int i = 0;
    int x, y;
    while(i < window_count){
        x = rand_with_range(1, COLS);
        y = rand_with_range(1, LINES);
        if(mvinch(y, x) == '_' || mvinch(y, x) == '|'){
            if(!corner(x, y)){
                mvprintw(y, x, "=");
                i++;
            }
        }
    }
}

void generate_floor_map(char *username, char difficulty, int floor_num){
    clear();

    struct room rooms[20];
    int room_count = 0;
    int min_rand, max_rand;
    if(difficulty == 'e'){
        min_rand = 6;
        max_rand = 8;
    }
    if(difficulty == 'm'){
        min_rand = 8;
        max_rand = 10;
    }
    if(difficulty == 'h'){
        min_rand = 10;
        max_rand = 12;
    }
    
    int all_rooms = rand_with_range(min_rand, max_rand);

    while(1){
        max_rand = 20;
        min_rand = 4;
        rooms[room_count].x_size = rand_with_range(min_rand, max_rand);
        max_rand = 10;
        min_rand = 4;
        rooms[room_count].y_size = rand_with_range(min_rand, max_rand);

        max_rand = COLS - rooms[room_count].x_size - 10;
        min_rand = 1;
        rooms[room_count].x_top_left = rand_with_range(min_rand, max_rand);
        max_rand = LINES - rooms[room_count].y_size - 10;
        min_rand = 1;
        rooms[room_count].y_top_left = rand_with_range(min_rand, max_rand);

        if(check_room(rooms, room_count)){
            print_room(rooms, room_count);
            room_count++;
        }

        if(room_count >= all_rooms){
            break;
        }
    }

    struct hallway hallways[room_count - 1];
    for(int i = 0; i < room_count - 1; i++){
        int room_1 = i;
        int room_2 = i + 1;

        hallways[i].x_1 = rooms[room_1].x_top_left + rand_with_range(2, rooms[room_1].x_size - 2);
        hallways[i].y_1 = rooms[room_1].y_top_left + rand_with_range(2, rooms[room_1].y_size - 2);
        hallways[i].x_2 = rooms[room_2].x_top_left + rand_with_range(2, rooms[room_2].x_size - 2);
        hallways[i].y_2 = rooms[room_2].y_top_left + rand_with_range(2, rooms[room_2].y_size - 2);
        
        print_hallway(hallways, i);
    }

    generate_windows();
    generate_pillars();
    
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            if(mvinch(y, x - 1) == '#' && mvinch(y, x) == '+' && mvinch(y, x + 1) == '+'){
                if(mvinch(y - 1, x) == '|' || mvinch(y - 1, x + 1) == '.'){
                    mvprintw(y + 1, x - 1, "#");
                    int i = 0;
                    while(mvinch(y, x + i) == '+'){
                        mvprintw(y, x + i, "_");
                        mvprintw(y + 1, x + i, "#");
                        i++;
                    }
                    mvprintw(y + 1, x + i, "#");
                    if(mvinch(y, x + i) == '_'){
                        mvprintw(y, x + i, "+");
                    }
                }

                if(mvinch(y + 1, x) == '|' || mvinch(y + 1, x + 1) == '.'){
                    mvprintw(y - 1, x - 1, "#");
                    int i = 0;
                    while(mvinch(y, x + i) == '+'){
                        mvprintw(y, x + i, "_");
                        mvprintw(y - 1, x + i, "#");
                        i++;
                    }
                    mvprintw(y - 1, x + i, "#");
                    if(mvinch(y, x + i) == '_'){
                        mvprintw(y, x + i, "+");
                    }
                }
            }
            if(mvinch(y, x + 1) == '#' && mvinch(y, x) == '+' && mvinch(y, x - 1) == '+'){
                if(mvinch(y - 1, x) == '|'){
                    mvprintw(y + 1, x - 1, "#");
                    int i = 0;
                    while(mvinch(y, x + i) == '+'){
                        mvprintw(y, x + i, "_");
                        mvprintw(y + 1, x + i, "#");
                        i--;
                    }
                    mvprintw(y + 1, x + i, "#");
                    if(mvinch(y, x + i) == '_'){
                        mvprintw(y, x + i, "+");
                    }
                }

                if(mvinch(y + 1, x) == '|'){
                    mvprintw(y - 1, x - 1, "#");
                    int i = 0;
                    while(mvinch(y, x + i) == '+'){
                        mvprintw(y, x + i, "_");
                        mvprintw(y + 1, x + i, "#");
                        i--;
                    }
                    mvprintw(y - 1, x + i, "#");
                    if(mvinch(y, x + i) == '_'){
                        mvprintw(y, x + i, "+");
                    }
                }
            }

            if(mvinch(y + 1, x) == '#' && mvinch(y, x) == '+' && mvinch(y - 1, x) == '+'){
                if(mvinch(y, x - 1) == '_'){
                    mvprintw(y, x + 1, "#");
                    int i = 0;
                    while(mvinch(y - i, x) == '+'){
                        mvprintw(y - i, x , "|");
                        mvprintw(y - i, x + 1, "#");
                        i++;
                    }
                    mvprintw(y - i, x + 1, "#");
                }

                if(mvinch(y, x + 1) == '_'){
                    mvprintw(y + 1, x - 1, "#");
                    int i = 0;
                    while(mvinch(y - i, x) == '+'){
                        mvprintw(y - i, x , "|");
                        mvprintw(y - i, x - 1, "#");
                        i++;
                    }
                    mvprintw(y + i, x - 1, "#");
                }
            }

            if(mvinch(y - 1, x) == '#' && mvinch(y, x) == '+' && mvinch(y + 1, x) == '+'){
                if(mvinch(y, x - 1) == '_'){
                    mvprintw(y, x + 1, "#");
                    int i = 0;
                    while(mvinch(y + i, x) == '+'){
                        mvprintw(y + i, x , "|");
                        mvprintw(y + i, x + 1, "#");
                        i++;
                    }
                    mvprintw(y + i, x + 1, "#");
                }

                if(mvinch(y, x + 1) == '_'){
                    mvprintw(y, x - 1, "#");
                    int i = 0;
                    while(mvinch(y + i, x) == '+'){
                        mvprintw(y + i, x , "|");
                        mvprintw(y + i, x - 1, "#");
                        i++;
                    }
                    mvprintw(y + i, x - 1, "#");
                }
            }
        }
    }

    char filename[100];
    snprintf(filename, sizeof(filename), "%s.txt", username);
    FILE *map_file = fopen(filename, "a");
    fprintf(map_file, "Room Count%d: %d\n", floor_num, room_count);
    fprintf(map_file, "Map%d:", floor_num);
    for(int y = 0; y < LINES; y++){
        for(int x = 0; x < COLS; x++){
            fprintf(map_file, "%c", mvinch(y, x) & A_CHARTEXT);
        }
        fprintf(map_file, "\n");
    }
    
    fclose(map_file);
    clear();
}

void generate_map(char *username, char difficulty){
    srand(time(0));
    for(int i = 1; i <= 4; i++){
        generate_floor_map(username, difficulty, i);
    }
}

#endif