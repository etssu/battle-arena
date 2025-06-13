#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include "graphics.h"
#include "logic.h"

extern int field_start_y;
extern int field_start_x;
int army1_size = 0;
int army2_size = 0;
extern UNIT *army_1;
extern UNIT *army_2;

int main() {
    initscr();              // инициализация ncurses
    start_color();          // включение цветов
    cbreak();               // отключить буферизацию ввода
    noecho();               // не отображать вводимые символы
    keypad(stdscr, TRUE);   // включить клавиши со стрелками

    while (1) {
        clear();
        init_menu_art();
        int choice = init_menu_buttons(); // теперь возвращает int

        if (choice == 0) { // Play
            clear();
            refresh();
            init_armies();
            auto_place_units(army_1, army1_size, army_2, army2_size);
            draw_field();
            
            display_armies(army_1, army1_size, army_2, army2_size);
            handle_turn(); 
            mvprintw(0, 0, "Press any key to return to menu...");
            getch();
        } else if (choice == 1) { // Rules
            rules(); // after rules return to menu
        } else if (choice == 2) { // Exit
            break; // exit the loop
        }
    }
    free(army_1); // free memory
    free(army_2);
    endwin(); // end ncurses mode
    return 0;
}