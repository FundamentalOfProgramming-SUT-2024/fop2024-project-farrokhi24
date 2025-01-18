#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct User{
    char username[50];
    int score;
    int gold;
    int games_played;
    long int experience;
};

void sort_by_score(struct User users[], int user_count){
    int check;
    for(int i = 0; i < user_count - 1; i++){
        check = 1;
        for(int j = 0; j < user_count - i - 1; j++){
            if(users[j].score < users[j + 1].score){
                struct User temp = users[j];
                users[j] = users[j + 1];
                users[j + 1] = temp;
                check = 0;
            }
        }
        if(check){
            break;
        }
    }
}

void print_leaderboard(struct User users[], int user_count, int page_number, char *username){
    int start_index =(page_number - 1) * 35;
    int end_index = start_index + 35;
    if(end_index > user_count){
        end_index = user_count;
    }

    start_color();

    attron(A_UNDERLINE | A_BOLD);
    mvprintw(3,(COLS - 10) / 2, "Leaderboard");
    mvprintw(4,(COLS + 40) / 2, "Page %d of %d", page_number,(user_count + 35 - 1) / 35);
    attroff(A_BOLD);

    mvprintw(6, (COLS - 64) / 2, "Rank | Username | Score | Gold | Games Played | Experience | Title");
    attroff(A_UNDERLINE);

    start_color();

    int y = 7;
    for(int i = start_index; i < end_index; i++){
        if(strcmp(username, users[i].username) == 0){
            attron(A_BOLD);
        }
        else{
            attroff(A_BOLD);
        }

        if(i == 0){
            attron(COLOR_PAIR(2) | A_ITALIC);
            mvprintw(y + i - start_index,(COLS - 64) / 2, "                                                                   ");
            mvprintw(y + i - start_index,(COLS + 58) / 2, "GOAT");
        }
        else if(i == 1){
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(3));
            mvprintw(y + i - start_index,(COLS - 64) / 2, "                                                                   ");
            mvprintw(y + i - start_index,(COLS + 56) / 2, "Legend");
        }
        else if(i == 2){
            attroff(COLOR_PAIR(3));
            attron(COLOR_PAIR(4));
            mvprintw(y + i - start_index,(COLS - 64) / 2, "                                                                   ");
            mvprintw(y + i - start_index,(COLS + 58) / 2, "Epic");
        }
        else{
            attroff(COLOR_PAIR(4) | A_ITALIC);
            mvprintw(y + i - start_index,(COLS + 62) / 2, "-");
        }
        
        mvprintw(y + i - start_index,(COLS - 60) / 2, "%d", i + 1);
        if(i == 0){
            mvprintw(y + i - start_index, (COLS - 50) / 2, "%s", users[i].username);
            printw("\U0001F947");
        }
        if(i == 1){
            mvprintw(y + i - start_index, (COLS - 50) / 2, "%s", users[i].username);
            printw("\U0001F948");
        }
        if(i == 2){
            mvprintw(y + i - start_index, (COLS - 50) / 2, "%s", users[i].username);
            printw("\U0001F949");
        }
        else{
            mvprintw(y + i - start_index,(COLS - 50) / 2, "%s", users[i].username);
        }
        mvprintw(y + i - start_index,(COLS - 24) / 2, "%d", users[i].score);
        mvprintw(y + i - start_index,(COLS - 10) / 2, "%d", users[i].gold);
        mvprintw(y + i - start_index,(COLS + 14) / 2, "%d", users[i].games_played);
        long int days = users[i].experience / 86400;
        float hours =(users[i].experience % 86400) / 3600.0;
        mvprintw(y + i - start_index,(COLS + 32) / 2, "%ldd %.2fh", days, hours);
        attroff(COLOR_PAIR(2));
    }

    curs_set(0);
    refresh();
}

void print_options(int highlight){
    char *options[] ={"Back", "Prev", "Next"};
    int spaces[] ={-50, 30, 50};
    for(int i = 0; i < 3; ++i){
        if(highlight == i + 1){
            attron(A_REVERSE);
            mvprintw(LINES - 2,(COLS + spaces[i]) / 2, "%s", options[i]);
            attroff(A_REVERSE);
        }
        else{
            mvprintw(LINES - 2,(COLS + spaces[i]) / 2, "%s", options[i]);
        }
    }
}

void read_all_users(struct User users[], int *user_count){
    FILE *file = fopen("user_data.txt", "r");

    char line[256];
    int i = 0;
    while(fgets(line, sizeof(line), file)){
        if(sscanf(line, "Username: %49s", users[i].username) == 1){
            char user_file_name[100];
            snprintf(user_file_name, sizeof(user_file_name), "%s.txt", users[i].username);
            FILE *user_file = fopen(user_file_name, "r");
            if(user_file == NULL){
                continue;
            }
            long int now = time(NULL);
            char user_line[256];
            while(fgets(user_line, sizeof(user_line), user_file)){
                if(sscanf(user_line, "Score: %d", &users[i].score) == 1) continue;
                if(sscanf(user_line, "Gold: %d", &users[i].gold) == 1) continue;
                if(sscanf(user_line, "Games Played: %d", &users[i].games_played) == 1) continue;
                long first_game_time;
                if(sscanf(user_line, "Time: %ld", &first_game_time) == 1){
                    users[i].experience = now - first_game_time;
                }
            }

            fclose(user_file);
            i++;
        }
    }

    fclose(file);
    *user_count = i;
}

void leaderboard(char *username){
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    int page_number = 1;
    int highlight = 1;
    int c;    

    struct User users[500];
    int user_count = 0;
    read_all_users(users, &user_count);

    sort_by_score(users, user_count);
    while(1){
        clear();
        print_leaderboard(users, user_count, page_number, username);
        print_options(highlight);

        c = getch();
        if(c == KEY_RIGHT){
            if(highlight == 3){
                highlight = 1;
            }
            else{
                highlight++;
            }
        }
        else if(c == KEY_LEFT){
            if(highlight == 1){
                highlight = 3;
            }
            else{
                highlight--;
            }
        }
        else if((c == 10) &&(highlight == 1)){
            break;
        }
        else if((c == 10) &&(highlight == 2)){
            if(page_number > 1){
                page_number--;
            }
            else{
                attron(COLOR_PAIR(1));
                mvprintw(0, 0, "You Are Already on the First Page.");
                attroff(COLOR_PAIR(1));
                getch();
            }
        }
        else if((c == 10) &&(highlight == 3)){
            if(page_number * 35 < user_count){
                page_number++;
            }
            else{
                attron(COLOR_PAIR(1));
                mvprintw(0, 0, "You Are Already on the Last Page.");
                attroff(COLOR_PAIR(1));
                getch();
            }
        }
    }

    clear();

    memset(users, 0, sizeof(users));
    endwin();
}
