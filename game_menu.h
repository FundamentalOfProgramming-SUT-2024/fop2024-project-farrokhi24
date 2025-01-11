#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "leaderboard.h"
#include "settings.h"
#include "generate_map.h"
#include "play.h"

void print_game_menu(int highlight, char **options){
    int y =(LINES - 9) / 2;
    start_color();


    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2,(COLS - 9) / 2, "Game Menu");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    for(int i = 0; i < 6; ++i){
        if(highlight == i + 1){
            attron(A_REVERSE);
            mvprintw(y,(COLS - strlen(options[i])) / 2, "%s", options[i]);
            attroff(A_REVERSE);
        }
        else{
            mvprintw(y,(COLS - strlen(options[i])) / 2, "%s", options[i]);
        }
        y += 2;
    }
    curs_set(0);
    refresh();
}

void new_game(char *username){
    char filename[100];
    strcpy(filename, username);
    strcat(filename, ".txt");

    FILE *file = fopen(filename, "r");
    if(file != NULL){
        fclose(file);
    }
    file = fopen(filename, "w");

    long int current_time = time(NULL);

    fprintf(file, "Score: 0\nGold: 0\nGames Played: 0\nTime: %ld\n",current_time);

    fclose(file);
}

int game_menu(char *username, char *color, char *difficulty){
    if(username == NULL){
        username = "Guest";
    }

    int highlight = 1;
    char *options[] ={"New Game", "Load Game", "Leaderboard", "Settings", "Profile", "Back to Main Menu"};

    keypad(stdscr, TRUE);
    refresh();
    print_game_menu(highlight, options);
    int choice = 0;
    int c;
    while(1){
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
        else if((c == 10) &&(highlight == 1)){
            new_game(username);
            generate_map(username, *difficulty);
            play(username, *color, *difficulty);
        }
        else if((c == 10) &&(highlight == 2)){
            play(username, *color, *difficulty);
        }
        else if((c == 10) &&(highlight == 3)){
            leaderboard(username);
        }
        else if((c == 10) &&(highlight == 4)){
            settings(color, difficulty);
        }
        else if((c == 10) &&(highlight == 5)){
            clear();
            mvprintw(LINES / 2 - 1, (COLS - 22 - strlen(username))/2, "You Are Signed in as %s.", username);
            mvprintw(LINES / 2 + 1, (COLS - 30) / 2, "Press Any Key to Continue...");
            getch();
            clear();
        }
        else if((c == 10) &&(highlight == 6)){
            break;
        }

        print_game_menu(highlight, options);
        if((choice != 0) &&(choice != 1)){
            break;
        }
    }
    clrtoeol();
    refresh();
    return choice;
}
