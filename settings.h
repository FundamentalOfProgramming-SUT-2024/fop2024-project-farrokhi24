#include <ncurses.h>
#include <string.h>
#include "music.h"

void print_settings_page(int highlight, char **options){
    int y = (LINES - 9) / 2;
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2, (COLS - 7) / 2, "Settings");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    for(int i = 0; i < 4; ++i){
        if(highlight == i + 1){
            attron(A_REVERSE);
            mvprintw(y, (COLS - strlen(options[i])) / 2, "%s", options[i]);
            attroff(A_REVERSE);
        }
        else{
            mvprintw(y, (COLS - strlen(options[i])) / 2, "%s", options[i]);
        }
        y += 2;
    }
    curs_set(0);
    refresh();
}

void print_difficulty_page(int highlight, char **options, char difficulty){
    int y = (LINES - 9) / 2;
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2, (COLS - 15) / 2, "Choose Difficulty:");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    for(int i = 0; i < 4; ++i){
        if(highlight == i + 1){
            attron(A_REVERSE);
        }
        if(i == 0 && difficulty == 'e'){
            attron(A_UNDERLINE);
        }
        else if(i == 1 && difficulty == 'm'){
            attron(A_UNDERLINE);
        }
        else if(i == 2 && difficulty == 'h'){
            attron(A_UNDERLINE);
        }
        mvprintw(y, (COLS - strlen(options[i])) / 2, "%s", options[i]);
        attroff(A_REVERSE | A_UNDERLINE);
        y += 2;
    }
    curs_set(0);
    refresh();
}

void choose_difficulty(char *difficulty){
    char *options[] = {"Easy", "Medium", "Hard", "Back"};
    int highlight = 1;
    int c;

    while(1){
        clear();
        print_difficulty_page(highlight, options, *difficulty);
        c = getch();
        if(c == KEY_UP){
            if(highlight == 1){
                highlight = 4;
            }
            else{
                highlight--;
            }
        }
        else if(c == KEY_DOWN){
            if(highlight == 4){
                highlight = 1;
            }
            else{
                highlight++;
            }
        }
        else if((c == 10) && (highlight == 1)){
            *difficulty = 'e';
        }
        else if((c == 10) && (highlight == 2)){
            *difficulty = 'm';
        }
        else if((c == 10) && (highlight == 3)){
            *difficulty = 'h';
        }
        else if((c == 10) && (highlight == 4)){
            break;
        }
        refresh();
    }

    clrtoeol();
    refresh();
}

void print_character_color_page(int highlight, char **options, char color){
    int y = (LINES - 9) / 2;
    start_color();

    init_pair(1, COLOR_RED, COLOR_BLACK);

    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2, (COLS - 15) / 2, "Choose Your Color:");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    for(int i = 0; i < 6; ++i){
        if(highlight == i + 1){
            attron(A_REVERSE);
        }
        if(i == 0 && color == 'w'){
            attron(A_UNDERLINE);
        }
        else if(i == 1 && color == 'r'){
            attron(A_UNDERLINE);
        }
        else if(i == 2 && color == 'b'){
            attron(A_UNDERLINE);
        }
        else if(i == 3 && color == 'g'){
            attron(A_UNDERLINE);
        }
        else if(i == 4 && color == 'y'){
            attron(A_UNDERLINE);
        }
        mvprintw(y, (COLS - strlen(options[i])) / 2, "%s", options[i]);
        attroff(A_REVERSE | A_UNDERLINE);
        y += 2;
    }
    curs_set(0);
    refresh();
}

void choose_character_color(char *color){
    char *options[] = {"White", "Red", "Blue", "Green", "Yellow", "Back"};
    int highlight = 1;
    int c;

    while(1){
        clear();
        print_character_color_page(highlight, options, *color);
        c = getch();
        if(c == KEY_UP){
            if(highlight == 1){
                highlight = 6;
            }
            else{
                highlight--;
            }
        }
        else if(c == KEY_DOWN){
            if(highlight == 6){
                highlight = 1;
            }
            else{
                highlight++;
            }
        }
        else if((c == 10) && (highlight == 1)){
            *color = 'w';
        }
        else if((c == 10) && (highlight == 2)){
            *color = 'r';
        }
        else if((c == 10) && (highlight == 3)){
            *color = 'b';
        }
        else if((c == 10) && (highlight == 4)){
            *color = 'g';
        }
        else if((c == 10) && (highlight == 5)){
            *color = 'y';
        }
        else if((c == 10) && (highlight == 6)){
            break;
        }
        refresh();
    }
    clrtoeol();
    refresh();
}

void settings(char *color, char *difficulty, int *song){
    char *options[] = {"Difficulty", "Character Color", "Music", "Exit"};
    int highlight = 1;
    int c;

    while(1){
        clear();
        print_settings_page(highlight, options);
        c = getch();
        if(c == KEY_UP){
            if(highlight == 1){
                highlight = 4;
            }
            else{
                highlight--;
            }
        }
        else if(c == KEY_DOWN){
            if(highlight == 4){
                highlight = 1;
            }
            else{
                highlight++;
            }
        }
        else if((c == 10) && (highlight == 1)){
            choose_difficulty(difficulty);
        }
        else if((c == 10) && (highlight == 2)){
            choose_character_color(color);
        }
        else if((c == 10) && (highlight == 3)){
            music(song);
        }
        else if((c == 10) && (highlight == 4)){
            break;
        }
        refresh();
    }
    clear();
    clrtoeol();
    refresh();
}