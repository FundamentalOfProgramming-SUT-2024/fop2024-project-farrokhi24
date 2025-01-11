#include <ncurses.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

void print_music_page(int highlight, char **options){
    int y = (LINES - 9) / 2;
    start_color();


    attron(A_UNDERLINE | A_BOLD);
    mvprintw(y - 2, (COLS - 5) / 2, "Music");
    attroff(A_UNDERLINE | A_BOLD);
    y++;
    for (int i = 0; i < 5; ++i){
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
    refresh();
}

void play_music(char *track_name){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    Mix_Music *music = Mix_LoadMUS(track_name);
    Mix_PlayMusic(music, -1);
}
void music(){
    char *options[] ={"Track 1", "Track 2", "Track 3", "Turn Music Off", "Exit"};
    int highlight = 1;
    int c;

    curs_set(0);

    while (1){
        clear();
        print_music_page(highlight, options);
        c = getch();
        if(c == KEY_UP){
            if(highlight == 1){
                highlight = 5;
            }
            else{
                highlight--;
            }
        }
        else if(c == KEY_DOWN){
            if(highlight == 5){
                highlight = 1;
            }
            else{
                highlight++;
            }
        }
        else if((c == 10) && (highlight == 1)){
            play_music("track_1.mp3");
        }
        else if((c == 10) && (highlight == 2)){
            play_music("track_2.mp3");
        }
        else if((c == 10) && (highlight == 3)){
            play_music("track_3.mp3");
        }
        else if((c == 10) && (highlight == 4)){
            Mix_HaltMusic();
        }
        else if((c == 10) && (highlight == 5)){
            break;
        }
        refresh();
    }

    clrtoeol();
    refresh();
}
