#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_signup_page(int highlight, char **options, char *username, char *password, char *email, int password_generated){
    int y = (LINES - 13) / 2;
    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2, (COLS - 15) / 2, "Create New User");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    
    mvprintw(y, (COLS - 15) / 2, "%s", options[0]);
    if(strlen(username) > 0){
        mvprintw(y, (COLS - 15) / 2 + strlen(options[0]), "%s", username);
    }
    y += 2;
    
    mvprintw(y, (COLS - 15) / 2, "%s", options[1]);
    if(strlen(password) > 0){
        if(password_generated){
            mvprintw(y, (COLS - 15) / 2 + strlen(options[1]), "%s", password);
        }
        else{
            for(int j = 0; j < strlen(password); ++j){
                mvaddch(y, (COLS - 15) / 2 + strlen(options[1]) + j, '*');
            }
        }
    }
    y += 2;
    
    mvprintw(y, (COLS - 15) / 2, "%s", options[2]);
    if(strlen(email) > 0){
        mvprintw(y, (COLS - 15) / 2 + strlen(options[2]), "%s", email);
    }
    y += 2;
    
    if(highlight == 4){
        attron(A_REVERSE);
        mvprintw(y, (COLS - strlen(options[3])) / 2, "%s", options[3]);
        attroff(A_REVERSE);
    } 
    else{
        mvprintw(y, (COLS - strlen(options[3])) / 2, "%s", options[3]);
    }
    y += 2;

    if(highlight == 5){
        attron(A_REVERSE);
        mvprintw(y, (COLS - strlen(options[4])) / 2, "%s", options[4]);
        attroff(A_REVERSE);
    } 
    else{
        mvprintw(y, (COLS - strlen(options[4])) / 2, "%s", options[4]);
    }
    y += 2;

    if(highlight == 6){
        attron(A_REVERSE);
        mvprintw(y, (COLS - strlen(options[5])) / 2, "%s", options[5]);
        attroff(A_REVERSE);
    } 
    else{
        mvprintw(y, (COLS - strlen(options[5])) / 2, "%s", options[5]);
    }

    curs_set(1);
    refresh();
}

void input_password(char *password, int max_length, int *check){
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
            *check = 1;
            break;
        }
        else{
            password[i++] = ch;
            addch('*');
        }
        refresh();
    }
    password[i] = '\0';
    echo();
}


int duplicate_username(char *username){
    FILE *file = fopen("user_data.txt", "r");

    char line[60];
    while(fgets(line, sizeof(line), file)){
        char read_username[50];
        sscanf(line, "Username: %s", read_username);
        if(strcmp(read_username, username) == 0){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int valid_password(char *password){
    if(strlen(password) < 7){
        return 0;
    }
    int lowercase = 0, uppercase = 0, digit = 0;
    for(int i = 0; password[i] != '\0'; i++){
        if(islower(password[i])){
            lowercase++;
        }
        if(isupper(password[i])){
            uppercase++;
        }
        if(isdigit(password[i])){
            digit++;
        }
    }
    if(uppercase && lowercase && digit){
        return 1;
    }
    return 0;
}

int valid_email(char *email){
    int atsign_index = -1, dot_index = -1;
    for(int i = 0; email[i] != '\0'; i++){
        if(email[i] == '@'){
            atsign_index = i;
        }
        if(email[i] == '.'){
            dot_index = i;
        }
    }
    if((atsign_index > 0) && (dot_index > atsign_index + 1) && (dot_index < strlen(email) - 1)){
        return 1;
    }
    return 0;
}

void generate_password(char *password, int length){
    srand(time(0));
    do{
        for(int i = 0; i < length; i++){
            int char_type = rand() % 3;
            if(char_type == 0){
                password[i] = 'a' + rand() % 26;
            }
            else if(char_type == 1){
                password[i] = 'A' + rand() % 26;
            }
            else{
                password[i] = '0' + rand() % 10;
            }
        }
        password[length] = '\0';
    } while(!valid_password(password));
}

void signup(){
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    int y = (LINES - 13) / 2;
    char username[50] = {0}, password[50] = {0}, email[50] = {0};

    char *options[] = {"Username: ", "Password: ", "Email: ", "Generate Random Password", "Enter", "Cancel"};

    int highlight = 1;
    int choice = 0;
    int c;
    int password_generated = 0;
    int check = 0;

    echo();
    keypad(stdscr, TRUE);
    curs_set(1);

    clear();
    refresh();
    print_signup_page(highlight, options, username, password, email, password_generated);

    move(y + 1, (COLS - 15) / 2 + strlen(options[0]));
    getstr(username);

    move(y + 3, (COLS - 15) / 2 + strlen(options[1]));
    input_password(password, 50, &check);

    if(check){
        highlight = 4;
    }

    move(y + 5, (COLS - 15) / 2 + strlen(options[2]));
    getstr(email);

    while(1){
        clear();
        print_signup_page(highlight, options, username, password, email, password_generated);
        
        if(strlen(username) == 0){
            attron(COLOR_PAIR(1));
            mvprintw(0, 0, "Please Enter a Username.");
            attroff(COLOR_PAIR(1));
            
            move(y + 1, (COLS - 15) / 2 + strlen(options[0]));
            getstr(username);
        }
        if(strlen(password) == 0 && !password_generated){
            attron(COLOR_PAIR(1));
            mvprintw(0, 0, "Please Enter a Password.");
            attroff(COLOR_PAIR(1));
            move(y + 3, (COLS - 15) / 2 + strlen(options[1]));
            input_password(password, 50, &check);
            if(check){
                highlight = 4;
            }
        }
        if(strlen(email) == 0){
            attron(COLOR_PAIR(1));
            mvprintw(0, 0, "Please Enter an Email.");
            attroff(COLOR_PAIR(1));
            move(y + 5, (COLS - 15) / 2 + strlen(options[2]));
            clrtoeol();
            getstr(email);
        }

        refresh();

        while(1){
            curs_set(0);
            noecho();
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
            else if((c == 10) && (highlight == 4)){
                generate_password(password, 7);
                password_generated = 1;
                break;
            } 
            else if((c == 10) && (highlight == 5)){
                if(strlen(username) == 0 || strlen(password) == 0 || strlen(email) == 0){
                    break;
                }
                if(duplicate_username(username)){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "Username Already Exists.");
                    attroff(COLOR_PAIR(1));
                    memset(username, 0, sizeof(username));
                    move(y + 1, (COLS - 15) / 2 + strlen(options[0]));
                    clrtoeol();
                    print_signup_page(highlight, options, username, password, email, password_generated);
                    move(y + 1, (COLS - 15) / 2 + strlen(options[0]));
                    getstr(username);
                    break;
                }
                if(!valid_password(password)){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "Password Should Have at Least 7 Characters, 1 Lowercase Character, 1 Uppercase Character and 1 Number.");
                    attroff(COLOR_PAIR(1));
                    memset(password, 0, sizeof(password));
                    move(y + 3, (COLS - 15) / 2 + strlen(options[1]));
                    clrtoeol();
                    print_signup_page(highlight, options, username, password, email, password_generated);
                    move(y + 3, (COLS - 15) / 2 + strlen(options[1]));
                    input_password(password, 50, &check);
                    if(check){
                        highlight = 4;
                    }
                    break;
                }
                if(!valid_email(email)){
                    attron(COLOR_PAIR(1));
                    mvprintw(0, 0, "Please Enter a Valid Email.");
                    attroff(COLOR_PAIR(1));
                    memset(email, 0, sizeof(email));
                    move(y + 5, (COLS - 15) / 2 + strlen(options[2]));
                    clrtoeol();
                    print_signup_page(highlight, options, username, password, email, password_generated);
                    move(y + 5, (COLS - 15) / 2 + strlen(options[2]));
                    getstr(email);
                    break;
                }
                choice = 1;
                break;
            }
            else if((c == 10) && (highlight == 6)){
                return;
            } 
            else{
                refresh();
            }
            move((LINES - 9) / 2 + (highlight - 1) * 2, (COLS - strlen(options[highlight - 1])) / 2 + strlen(options[highlight - 1]));
            print_signup_page(highlight, options, username, password, email, password_generated);
        }

        if(choice == 1){
            break;
        }
    }

    if(choice == 1){
        clear();
        
        FILE *file = fopen("user_data.txt", "a");
        fprintf(file, "Username: %s\n", username);
        fprintf(file, "Password: %s\n", password);
        fprintf(file, "Email: %s\n\n", email);
        fclose(file);

        curs_set(0);
        mvprintw(LINES / 2 - 1, (COLS - 9 - strlen(username)) / 2, "Welcome, %s!", username);
        mvprintw(LINES / 2 + 1, (COLS - 24) / 2, "Press Any Key to Continue...");

        refresh();
        getch();
    }

    curs_set(0);
    clrtoeol();
    refresh();
}