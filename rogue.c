#include <ncurses.h>
#include <locale.h>
#include "menu.h"
#include "signup.h"
#include "login.h"
#include "game_menu.h"

#define COLOR_DARK_GOLD 11
#define COLOR_DARK_SILVER 12
#define COLOR_BRONZE 13
#define COLOR_ORANGE 14
#define COLOR_PURPLE 15
#define COLOR_BROWN 16
#define COLOR_GRAY 17
#define COLOR_PINK 18
#define COLOR_SILVER 19
#define COLOR_GOLD 20
#define COLOR_DARK_PINK 21
#define COLOR_LIGHT_GRAY 22

int main(){
    setlocale(LC_ALL, "");
    initscr();
    clear();

    start_color();
    init_color(COLOR_DARK_GOLD, 700, 500, 100);
    init_color(COLOR_DARK_SILVER, 350, 350, 350);
    init_color(COLOR_BRONZE, 600, 300, 75);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_ORANGE, 900, 500, 0);
    init_color(COLOR_PURPLE, 600, 0, 600);
    init_color(COLOR_BROWN, 823, 549, 431);
    init_color(COLOR_GRAY, 300, 300, 300);
    init_color(COLOR_LIGHT_GRAY, 350, 350, 350);
    init_color(COLOR_PINK, 900, 0, 750);
    init_color(COLOR_SILVER, 600, 600, 600);
    init_color(COLOR_GOLD, 800, 600, 200);
    init_color(COLOR_DARK_PINK, 600, 0, 450);

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_DARK_GOLD);
    init_pair(3, COLOR_WHITE, COLOR_DARK_SILVER);
    init_pair(4, COLOR_WHITE, COLOR_BRONZE);
    //init_pair(5, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_GOLD, COLOR_BLACK);
    init_pair(8, COLOR_RED, COLOR_BLACK);
    init_pair(9, COLOR_CYAN, COLOR_BLACK);
    init_pair(10, COLOR_GREEN, COLOR_BLACK);
    init_pair(11, COLOR_YELLOW, COLOR_BLACK);
    init_pair(12, COLOR_ORANGE, COLOR_BLACK);
    init_pair(13, COLOR_PURPLE, COLOR_BLACK);
    init_pair(14, COLOR_BROWN, COLOR_BLACK); 
    init_pair(15, COLOR_GRAY, COLOR_BLACK);
    init_pair(16, COLOR_SILVER, COLOR_BLACK);
    init_pair(18, COLOR_PINK, COLOR_BLACK);
    init_pair(20, COLOR_BLACK, COLOR_BLACK);
    init_pair(21, COLOR_WHITE, COLOR_BLACK);
    init_pair(22, COLOR_DARK_PINK, COLOR_BLACK);
    init_pair(23, COLOR_LIGHT_GRAY, COLOR_BLACK);

    noecho();
    int choice;
    char *username = NULL;
    char color = 'w';
    char difficulty = 'm';
    while(1){
        clear();
        choice = menu();
        clear();
        if(choice == 1){
            signup();
        }
        if(choice == 2){
            if(username != NULL){
                free(username);
                username = NULL;
            }
            username = login();
            printw("%s", username);
        }
        if(choice == 3){
            game_menu(username, &color, &difficulty);
        }
        if(choice == 4){
            break;
        }
    }

    endwin();
    return 0;
}