#include <ncurses.h>
#include <string.h>

void print_login_page(int highlight, char **options, char *username, char *password){
    int y =(LINES - 13) / 2;
    int x_labels =(COLS - 20) / 2;
    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2,(COLS - 5) / 2, "Login");
    attroff(A_UNDERLINE | A_BOLD);
    y++;

    for(int i = 0; i < 6; i++){
        if(i < 2){
            mvprintw(y, x_labels, "%s", options[i]);
            if(i == 0 && strlen(username) > 0){
                mvprintw(y, x_labels + strlen(options[i]), "%s", username);
            }
            else if(i == 1 && strlen(password) > 0){
                for(int j = 0; j < strlen(password); ++j){
                    mvaddch(y, x_labels + strlen(options[i]) + j, '*');
                }
            }
        }
        else{
            int x_options =(COLS - strlen(options[i])) / 2;
            if(highlight == i + 1){
                attron(A_REVERSE);
                mvprintw(y, x_options, "%s", options[i]);
                attroff(A_REVERSE);
            }
            else{
                mvprintw(y, x_options, "%s", options[i]);
            }
        }
        y += 2;
    }

    refresh();
}

void input_login_password(char *password, int max_length, int *exit_flag){
    noecho();
    int ch, i = 0;
    while((ch = getch()) != '\n' && i < max_length - 1){
        if(ch == KEY_BACKSPACE || ch == 127){
            if(i > 0){
                i--;
                mvaddch(getcury(stdscr), getcurx(stdscr) - 1, ' ');
                move(getcury(stdscr), getcurx(stdscr) - 1);
            }
        } 
        else if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT){
            *exit_flag = 1;
            break;
        }
        else{
            password[i] = ch;
            i++;
            printw("*");
        }
        refresh();
    }
    password[i] = '\0';
    echo();
}

int correct_info(char *username, char *password){
    FILE *file = fopen("user_data.txt", "r");
    if(file == NULL){
        return 0;
    }

    char line[60];
    while(fgets(line, sizeof(line), file)){
        char read_username[50], read_password[50];
        sscanf(line, "Username: %s", read_username);
        sscanf(line, "Password: %s", read_password);
        if((strcmp(read_username, username) == 0) &&(strcmp(read_password, password) == 0)){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int recover_password(char *email, char *password){
    FILE *file = fopen("user_data.txt", "r");

    char line[60];
    while(fgets(line, sizeof(line), file)){
        char read_email[50], read_password[50];
        sscanf(line, "Email: %s", read_email);
        fgets(line, sizeof(line), file);
        sscanf(line, "Password: %s", read_password);
        if(strcmp(read_email, email) == 0){
            strcpy(password, read_password);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

char *login(){
    int y =(LINES - 13) / 2;
    int x_labels =(COLS - 20) / 2;
    char username[50] = {0}, password[50] = {0}, email[50] = {0};
    char *options[] = {"Username: ", "Password: ", "Enter", "Forgot Password?", "Enter as Guest", "Cancel"};
    int highlight = 1;
    int choice = 0;
    int c;
    int invalid = 0;
    int exit_flag = 0;

    echo();
    keypad(stdscr, TRUE);
    curs_set(1);
    start_color();
    clear();
    refresh();
    print_login_page(highlight, options, username, password);
    move(y + 1, x_labels + strlen(options[0]));
    getstr(username);
    move(y + 3, x_labels + strlen(options[1]));
    input_login_password(password, 50, &exit_flag);

    if(exit_flag){
        highlight = 3;
    }
    curs_set(0);

    while(1){
        clear();
        print_login_page(highlight, options, username, password);
        if(invalid){
            attron(COLOR_PAIR(1));
            mvprintw(0, 0, "Wrong Username or Password.");
            attroff(COLOR_PAIR(1));
        }
        if(highlight <= 2){
            curs_set(1);
        }
        else{
            curs_set(0);
        }
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
        else if((c == 10) &&(highlight == 3)){
            if(!correct_info(username, password)){
                invalid = 1;
            }
            else{
                invalid = 0;
                clear();
                mvprintw(LINES / 2 - 1,(COLS - 15 - strlen(username)) / 2, "Welcome Back, %s!", username);
                break;
            }
        }
        else if((c == 10) &&(highlight == 4)){
            clear();
            mvprintw(LINES / 2 - 3,(COLS - 30) / 2, "Enter Your Email: ");
            getstr(email);
            if(recover_password(email, password)){
                mvprintw(LINES / 2 - 1,(COLS - 30) / 2, "Your Password is: %s", password);
            }
            else{
                attron(COLOR_PAIR(1));
                mvprintw(LINES / 2 - 1,(COLS - 30) / 2, "Email Not Found.");
                attroff(COLOR_PAIR(1));
                highlight = 1;
            }
            break;
        }
        else if((c == 10) &&(highlight == 5)){
            strcpy(username, "Guest");
            clear();
            mvprintw(LINES / 2 - 1,(COLS - 15) / 2, "Welcome, Guest!");
            break;
        }
        else if((c == 10) &&(highlight == 6)){
            return NULL;
        }
        refresh();
    }

    mvprintw(LINES / 2 + 1,(COLS - 30) / 2, "Press Any Key to Continue...");
    getch();
    clrtoeol();
    refresh();

    char *logged_in_username =(char*) malloc(strlen(username) + 1);
    strcpy(logged_in_username, username);
    return logged_in_username;
}
