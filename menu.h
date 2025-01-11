#include <ncurses.h>
#include <string.h>

void print_menu(int highlight, char **options){
    int y = (LINES - 9) / 2;
    start_color();


    attron(A_UNDERLINE | A_BOLD | COLOR_PAIR(1));
    mvprintw(y - 4, (COLS - 5) / 2, "ROGUE");
    attroff(COLOR_PAIR(1));
    mvprintw(y - 2, (COLS - 9) / 2, "Main Menu");
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

int menu(){
    int highlight = 1;

    char *options[] ={"Create New User", "Log in", "Game Menu", "Exit"};

    keypad(stdscr, TRUE);
    refresh();
    print_menu(highlight, options);
    int choice = 0;
    int c;
    while(1){
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
        else if(c == 10){
            choice = highlight;
        }
        else{
            refresh();
        }

        print_menu(highlight, options);
        if(choice != 0){
            break;
        }
    }
    clrtoeol();
    refresh();
    return choice;
}