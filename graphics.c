#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "logic.h"
#include "data.h"

#define MAX_UNITS 5

int cell_height;
int cell_width;
int field_height;
int field_width;
int start_y;
int start_x;
void rules();


void init_menu_art(){
    const char *ascii_art[] = {
        ".----------------------------------------------------------------.",
        "|  ____        _   _   _              _                          |",
        "| | __ )  __ _| |_| |_| | ___        / \\   _ __ ___ _ __   __ _  |",
        "| |  _ \\ / _` | __| __| |/ _ \\      / _ \\ | '__/ _ \\ '_ \\ / _` | |",
        "| | |_) | (_| | |_| |_| |  __/     / ___ \\| | |  __/ | | | (_| | |",
        "| |____/ \\__,_|\\__|\\__|_|\\___|    /_/   \\_\\_|  \\___|_| |_|\\__,_| |",
        "'----------------------------------------------------------------'",
    };
    int ascii_lines = 7; // number of lines in the ascii art
    int ascii_width = 0;
    for (int i = 0; i < ascii_lines; i++) {
        int len = strlen(ascii_art[i]);
        if (len > ascii_width) {
            ascii_width = len;
        }
    }

    int height, width;
    getmaxyx(stdscr, height, width); // getting the size of the window
    int start_y = height / 4;
    int start_x = (width - ascii_width) / 2;    // cetred

    for (int i = 0; i < ascii_lines; i++) {             
        mvprintw(start_y + i, start_x, "%s", ascii_art[i]);   // printing ascii art
    } 
    refresh();
}
int init_menu_buttons() {
    const char *menu_buttons[] = {"Play", "Rules", "Exit"}; // define the button labels
    int num_buttons = sizeof(menu_buttons) / sizeof(menu_buttons[0]);
    int selected_button = 0;
    int ch;

    start_color(); // initialize colors
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // standard button color
    init_pair(2, COLOR_WHITE, COLOR_BLUE); //selected button color

    int height, width;
    getmaxyx(stdscr, height, width);
    //buttons info
    int button_width = 13;
    int button_height = 3;
    int spacing = 1;
    int total_height = num_buttons * (button_height + spacing) - spacing;
    //starting position to center the buttons
    int start_y = (height - total_height) / 2 + 5;
    int start_x = (width - button_width) / 2;

    keypad(stdscr, TRUE); //enable keypad input

    while (1) { //handle menu interaction
        for (int i = 0; i < num_buttons; i++) {
            int y = start_y + i * (button_height + spacing); //calculate y position for each button
            WINDOW *button_window = newwin(button_height, button_width, y, start_x); //create a new window for each button
            box(button_window, 0, 0);
            int text_x = (button_width - strlen(menu_buttons[i])) / 2; //center the text
            wbkgd(button_window, (i == selected_button) ? COLOR_PAIR(2) : COLOR_PAIR(1)); //background color
            wattron(button_window, A_BOLD);
            mvwprintw(button_window, 1, text_x, "%s", menu_buttons[i]); //print the button text in bold
            wattroff(button_window, A_BOLD);
            wrefresh(button_window); //show the button
            delwin(button_window);
        }

        ch = getch(); //get user input
        if (ch == KEY_UP) { //move up
            selected_button = (selected_button - 1 + num_buttons) % num_buttons; 
        } else if (ch == KEY_DOWN) { //move down
            selected_button = (selected_button + 1) % num_buttons;
        } else if (ch == '\n' || ch == KEY_ENTER) {
            return selected_button;
        }
    }
}
void rules() {
    int win_height = 17;
    int win_width = 60;

    // get the size of the terminal window
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // center the window
    int start_y = (max_y - win_height) / 2;
    int start_x = (max_x - win_width) / 2;

    WINDOW *rules_win = newwin(win_height, win_width, start_y, start_x);
    box(rules_win, 0, 0);

    wattron(rules_win, A_BOLD | A_UNDERLINE);
    mvwprintw(rules_win, 1, 2, "Rules: Battle Arena");
    wattroff(rules_win, A_BOLD | A_UNDERLINE);

    attron(A_BOLD);
    mvwprintw(rules_win, 3, 2, "Main Goal:");
    attroff(A_BOLD);
    mvwprintw(rules_win, 4, 4, "- Defeat the enemy army.");

    attron(A_BOLD);
    mvwprintw(rules_win, 6, 2, "Army Setup:");
    attroff(A_BOLD);
    mvwprintw(rules_win, 7, 4, "- Choose the number of units.");
    mvwprintw(rules_win, 8, 4, "- Name each unit.");
    mvwprintw(rules_win, 9, 4, "- Assign up to 2 items per unit.");

    attron(A_BOLD);
    mvwprintw(rules_win, 11, 2, "Gameplay:");
    attroff(A_BOLD);
    mvwprintw(rules_win, 12, 4, "- Each turn: move OR attack.");
    mvwprintw(rules_win, 13, 4, "- You may also surrender.");

    mvwprintw(rules_win, win_height - 2, 2, "Press any key to return...");

    wrefresh(rules_win);
    getch(); // wait for user input

    delwin(rules_win); // delete the window
    touchwin(stdscr);  // refresh the main window
    refresh();
}
void draw_field() { // Function to create the game field
    // initialize colors
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // middle border
    init_pair(2, COLOR_RED, COLOR_BLACK);   // army 1 (A)
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // army 2 (B)

    cell_height = 4;
    cell_width = 7;
    field_height = FIELD_HEIGHT * cell_height + 1;
    field_width = FIELD_WIDTH * cell_width + 1;

    int height, width;
    getmaxyx(stdscr, height, width);
    start_y = (height - field_height) / 2; //center the field on the screen
    start_x = (width - field_width) / 2;

    // Draw the grid
    for (int i = 0; i <= FIELD_HEIGHT; i++) {
        for (int j = 0; j <= FIELD_WIDTH; j++) {
            int y = start_y + i * cell_height;
            int x = start_x + j * cell_width;
            mvaddch(y, x, '+');
            if (i < FIELD_HEIGHT) {
                for (int k = 1; k < cell_height; k++)
                    mvaddch(y + k, x, '|');
            }
            if (j < FIELD_WIDTH) {
                for (int k = 1; k < cell_width; k++)
                    mvaddch(y, x + k, '-');
            }
        }
    }

    // The center divider line (colored)
    int center_col = FIELD_WIDTH / 2;
    int line_x = start_x + center_col * cell_width;
    attron(COLOR_PAIR(1) | A_BOLD);
    for (int i = 0; i < field_height; i++) {
        mvaddch(start_y + i, line_x, ACS_CKBOARD);
    }
    attroff(COLOR_PAIR(1) | A_BOLD);

    // === DRAW UNITS ===
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            UNIT* unit = battlefield[y][x];
            if (unit && unit->hp > 0) {
                int draw_y = start_y + y * cell_height + cell_height / 2;
                int draw_x = start_x + x * cell_width + cell_width / 2;

                int index = -1;
                // check if the unit belongs to army 1
                if (is_unit_from_army(unit, army_1, army1_size)) {
                    for (int i = 0; i < army1_size; i++) {
                        if (&army_1[i] == unit) {
                            index = i;
                            attron(COLOR_PAIR(2) | A_BOLD);
                            break;
                        }
                    }
                } else if (is_unit_from_army(unit, army_2, army2_size)) { // check if the unit belongs to army 2
                    for (int i = 0; i < army2_size; i++) {
                        if (&army_2[i] == unit) {
                            index = i;
                            attron(COLOR_PAIR(3) | A_BOLD);
                            break;
                        }
                    }
                }
                // draw the unit's index in the cell
                if (index != -1) {
                    mvprintw(draw_y, draw_x - 1, "%2d", index);  // рисуем индекс
                    attroff(COLOR_PAIR(2) | COLOR_PAIR(3) | A_BOLD);
                }
            }
        }
    }
    refresh();
}
void display_armies(UNIT army1[], int size1, UNIT army2[], int size2) {
    int height, width;
    getmaxyx(stdscr, height, width);

    int box_width = 40;
    int box_height = size1 > size2 ? size1 * 5 + 2 : size2 * 5 + 2;
    //create boxes
    WINDOW *left_box = newwin(box_height, box_width, 1, 1);
    WINDOW *right_box = newwin(box_height, box_width, 1, width - box_width - 1);


    box(left_box, 0, 0);
    box(right_box, 0, 0);

    start_color(); // initialize colors
    init_pair(1, COLOR_RED, COLOR_BLACK); // color pair for dead units
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // color pair for alive units
    //army1
    for (int i = 0; i < size1; i++) {
        int y = i * 5 + 1;
        if (army1[i].hp > 0) {
            wattron(left_box, COLOR_PAIR(2)); // color for alive units
            mvwprintw(left_box, y, 2,  "Unit: %s, HP: %d", army1[i].name, army1[i].hp);

            //item 1
            mvwprintw(left_box, y + 1, 4, "- %s, Att: %d, Range: %d", army1[i].item1->name, army1[i].item1->att, army1[i].item1->range);
            
            if (army1[i].item2 != NULL) { // check if item2 is not NULL
            mvwprintw(left_box, y + 2, 4, "- %s, Att: %d, Range: %d", army1[i].item2->name, army1[i].item2->att, army1[i].item2->range);
            }
            wattroff(left_box, COLOR_PAIR(2));
        } else {
            wattron(left_box, COLOR_PAIR(1)); // color for dead units
            mvwprintw(left_box, y, 2,  "Unit: %s [dead]", army1[i].name);
            wattroff(left_box, COLOR_PAIR(1));
        }
    }

    //army2
    for (int i = 0; i < size2; i++) {
        int y = i * 5 + 1;
        if (army2[i].hp > 0) {
            wattron(right_box, COLOR_PAIR(2)); // color for alive units
            mvwprintw(right_box, y, 2,  "Unit: %s, hp: %d", army2[i].name, army2[i].hp);

            //item 1
            mvwprintw(right_box, y + 1, 4, "- %s, Att: %d, Range: %d", army2[i].item1->name, army2[i].item1->att, army2[i].item1->range);
            
            if (army2[i].item2 != NULL) { // check if item2 is not NULL
            mvwprintw(right_box, y + 2, 4, "- %s, Att: %d, Range: %d", army2[i].item2->name, army2[i].item2->att, army2[i].item2->range);
            }
            wattroff(right_box, COLOR_PAIR(2));
        } else {
            wattron(right_box, COLOR_PAIR(1)); // color for dead units
            mvwprintw(right_box, y, 2,  "Unit: %s [dead]", army2[i].name);
            wattroff(right_box, COLOR_PAIR(1));
        }        
    }
    wrefresh(left_box); // refresh the left box
    wrefresh(right_box); // refresh the right box
    delwin(left_box); // delete the left box
    delwin(right_box); // delete the right box
}
void highlight_cell(int y, int x, int cell_height, int cell_width, int start_y, int start_x, int color_pair) {
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED); // red if we can attack
    init_pair(2, COLOR_WHITE, COLOR_BLUE); // unit is selected
    init_pair(3, COLOR_WHITE, COLOR_YELLOW); // aoe dmg
    init_pair(4, COLOR_WHITE, COLOR_GREEN); // cell is selected
    int top = start_y + y * cell_height;
    int left = start_x + x * cell_width;

    attron(COLOR_PAIR(color_pair)); // Цвет подсветки

    // рисуем рамку вокруг клетки
    for (int i = 0; i < cell_width; i++) {
        mvprintw(top, left + i, "-");                           // top
        mvprintw(top + cell_height - 1, left + i, "-");         // bottom
    }
    for (int i = 1; i < cell_height - 1; i++) {
        mvprintw(top + i, left, "|");                           // left
        mvprintw(top + i, left + cell_width - 1, "|");          // right
    }
    attroff(COLOR_PAIR(color_pair));
}
int is_valid_cell(int x, int y) {
    return x >= 0 && x < FIELD_WIDTH && y >= 0 && y < FIELD_HEIGHT;
}
void highlight_attackable_targets(UNIT *unit, UNIT *enemy_army, int enemy_size, int start_y, int start_x) {
    int range1 = unit->item1 ? unit->item1->range : 0;
    int range2 = unit->item2 ? unit->item2->range : 0;
    int range = (range1 > range2) ? range1 : range2;

    for (int i = 0; i < enemy_size; i++) {
        UNIT *target = &enemy_army[i];

        if (target->hp <= 0) continue; // dead unit

        // Manhattan distance
        int dist = abs(unit->x - target->x) + abs(unit->y - target->y);

        if (dist <= range) {
            // highlight the target cell in red
            highlight_cell(target->y, target->x, cell_height, cell_width, start_y, start_x, 1);
        }
    }
}