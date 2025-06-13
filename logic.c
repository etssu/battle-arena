#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "logic.h"
#include "data.h" 
#include "graphics.h"

#define MAX_UNITS 5
#define NUM_WEAPONS 16
#define MAX_SLOTS 2

UNIT * battlefield[FIELD_HEIGHT][FIELD_WIDTH];
UNIT *army_1;  
UNIT *army_2;
extern int army1_size;
extern int army2_size;

void ascii_won_1(); // ASCII art for Army 1 victory
void ascii_won_2(); // ASCII art for Army 2 victory
bool check_victory(UNIT army1[], int size1, UNIT army2[], int size2);
void set_unit_name(UNIT *unit);
void select_weapons(UNIT *unit);
int select_number_of_units(int army_num);
void attack_army(UNIT *attacker, UNIT *enemy_army, int size_b);

int army1_defeated() {
    for (int i = 0; i < army1_size; i++) {
        if (army_1[i].hp > 0) {
            return 0; // at least one alive
        }
    }
    return 1; // all dead
}
int army2_defeated() {
    for (int i = 0; i < army2_size; i++) {
        if (army_2[i].hp > 0) {
            return 0; // at least one alive
        }
    }
    return 1; // all dead
}
bool is_unit_from_army(UNIT* u, UNIT* army, int army_size) { // check if the unit is from the army
    for (int i = 0; i < army_size; i++) {
        if (&army[i] == u) return true;
    }
    return false;
}
int max(int a, int b) { //searching for the maximum
    return (a > b) ? a : b;
}
int can_move_to(UNIT *unit, int x, int y) { //if a unit can move to a cell
    if (x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT) return 0;
    if (battlefield[y][x] != NULL) return 0; // cell is occupied
    return 1;
}
void init_armies(){ //initialize armies
    army1_size = select_number_of_units(1);
    army_1 = (UNIT *)malloc(army1_size * sizeof(UNIT)); // allocate memory for army 1

    army2_size = select_number_of_units(2);
    army_2 = (UNIT *)malloc(army2_size * sizeof(UNIT)); // allocate memory for army 2
    if (army_1 == NULL || army_2 == NULL) {
         mvprintw(1,1, "MEMORY ALLOCATION ERROR");
        exit(1);  // end the program if memory allocation fails
    }
    for (int i = 0; i < army1_size; i++) {
        clear();
        mvprintw(0, 0, "Configuring unit %d/%d for Army 1", i + 1, army1_size);
        refresh();
        army_1[i].hp = 100;     // initialize health points
        set_unit_name(&army_1[i]);
        select_weapons(&army_1[i]);
    }
    for (int i = 0; i < army2_size; i++) {
        clear();
        mvprintw(0, 0, "Configuring unit %d/%d for Army 2", i + 1, army2_size);
        refresh();
        army_2[i].hp = 100; // Initialize health points
        set_unit_name(&army_2[i]);
        select_weapons(&army_2[i]);
    }
}
int select_number_of_units(int army_num) { 
    int number_of_units = 1;  // start with 1 unit
    int ch;
    int height, width;
    getmaxyx(stdscr, height, width); // getting the size of the window

    WINDOW *win = newwin(10, 50, (height - 8) / 2, (width - 50) / 2);
    keypad(win, TRUE); // enable keypad input

    while (1) {
        // create a new window for the number of units
        wattron(win, A_BOLD);
        box(win, 0, 0);  // draw a box around the window
        mvwprintw(win, 1, 1, "Select number of units (1-5) for Army %d: ", army_num);
        mvwprintw(win, 3, 1, "Press UP/DOWN to change, ENTER to confirm.");
        mvwprintw(win, 5, 1, "Current selection: %d", number_of_units);
        wattroff(win, A_BOLD);

        wrefresh(win);

        ch = getch(); // get user input
        if (ch == KEY_UP) { 
            if (number_of_units < MAX_UNITS) {
                number_of_units++;
            }
        } else if (ch == KEY_DOWN) {
            if (number_of_units > 1) {
                number_of_units--;
            }
        } else if (ch == '\n' || ch == KEY_ENTER) { // Enter key
            return number_of_units;  // return the selected number of units
        }
    }
}
void set_unit_name(UNIT *unit) {
    int height, width;
    getmaxyx(stdscr, height, width);

    WINDOW *win = newwin(5, 40, (height - 5) / 2, (width - 40) / 2);
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "Enter the name of the unit: ");
    wrefresh(win);

    echo();  // Enable text input
    mvwgetnstr(win, 2, 2, unit->name, sizeof(unit->name) - 1); // get the name
    noecho();  // Disable text input

    delwin(win);
    clear();
    refresh();
}
void select_weapons(UNIT *unit) {
    int height, width;
    getmaxyx(stdscr, height, width);

    int win_height = NUM_WEAPONS + 8;
    int win_width = 70;
    WINDOW *win = newwin(win_height, win_width, (height - win_height) / 2, (width - win_width) / 2);
    keypad(win, TRUE);

    int weapon_choice = 0;

    int current_slots = 0;

    // check the current slots used by the unit
    if (unit->item1 != NULL) {
        current_slots += unit->item1->slots;
    }
    if (unit->item2 != NULL) {
        current_slots += unit->item2->slots;
    }
    start_color(); // start color functionality
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // color pair for the unselected weapon
    init_pair(2, COLOR_WHITE, COLOR_BLUE); // color pair for the selected weapon
    init_pair(3, COLOR_GREEN, COLOR_BLACK); // color pair for the selected item

    while (current_slots < MAX_SLOTS) {
        werase(win);
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Select weapons (max total 2 slots):");

        // Print all available weapons
        for (int i = 0; i < NUM_WEAPONS; i++) {
            int is_selected = ((unit->item1 != NULL && unit->item1 == &items[i]) || 
                            (unit->item2 != NULL && unit->item2 == &items[i]));
             // if the item cannot be added
            int is_dimmed = (current_slots + items[i].slots > MAX_SLOTS || 
                            (items[i].slots == 2 && (unit->item1 != NULL || unit->item2 != NULL)));

            // Select color based on the item state
            if (is_selected) {
                wattron(win, COLOR_PAIR(3));  // Green for selected item
            } else if (i == weapon_choice) {
                wattron(win, COLOR_PAIR(2));  // Blue for selected
            } else {
                wattron(win, COLOR_PAIR(1));  // Standard color for unselected
            }

            // If the item is dimmed, we apply dim attribute
            if (is_dimmed) {
                wattron(win, A_DIM);
            }

            // 
            if (i == weapon_choice) {
                wattron(win, A_REVERSE);
            }

            // Print the item name and its attributes
            mvwprintw(win, i + 3, 2, "%d. %s Att: %d, Def: %d, Slots: %d, Range: %d, Radius: %d",
                    i + 1, items[i].name, items[i].att, items[i].def,
                    items[i].slots, items[i].range, items[i].radius);

            // deleting text attributes
            wattroff(win, A_REVERSE);
            wattroff(win, A_DIM);
            wattroff(win, COLOR_PAIR(1));
            wattroff(win, COLOR_PAIR(2));
            wattroff(win, COLOR_PAIR(3));
        }

        mvwprintw(win, height - 2, 2, "Use arrows to select, ENTER to confirm");
        wrefresh(win);  // Обновляем окно для отображения изменений

        int ch = wgetch(win);  // Используем wgetch, так как работаем с конкретным окном
        if (ch == KEY_UP) {
            weapon_choice = (weapon_choice - 1 + NUM_WEAPONS) % NUM_WEAPONS;  // Перемещение вверх
        } else if (ch == KEY_DOWN) {
            weapon_choice = (weapon_choice + 1) % NUM_WEAPONS;  // Перемещение вниз
        } else if (ch == '\n' || ch == KEY_ENTER) {
            // Выбираем оружие, если оно может быть добавлено
            if (current_slots + items[weapon_choice].slots <= MAX_SLOTS) {
                // Если в инвентаре еще есть место для первого предмета
                if (unit->item1 == NULL) {
                    unit->item1 = &items[weapon_choice];  // Добавляем предмет в слот 1
                }
                // Если в инвентаре есть место для второго предмета
                else if (unit->item2 == NULL) {
                    unit->item2 = &items[weapon_choice];  // Добавляем предмет в слот 2
                    
                }
                current_slots += items[weapon_choice].slots;  // Увеличиваем количество занятых слотов
            }
        }
    }
    delwin(win);  // Закрываем окно
    clear();  // Очищаем экран
    refresh();  // Обновляем экран
}
void auto_place_units(UNIT army1[], int size1, UNIT army2[], int size2) {
    // Clear battlefield
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            battlefield[y][x] = NULL;
        }
    }
    // Army 1 – left side
    int index = 0;
    for (int x = 0; x < FIELD_WIDTH / 2 && index < size1; x++) {
        for (int y = 0; y < FIELD_HEIGHT && index < size1; y++) {
            army1[index].x = x;
            army1[index].y = y;
            battlefield[y][x] = &army1[index];
            index++;
        }
    }
    // Army 2 – right side
    index = 0;
    for (int x = FIELD_WIDTH - 1; x >= FIELD_WIDTH / 2 && index < size2; x--) {
        for (int y = 0; y < FIELD_HEIGHT && index < size2; y++) {
            army2[index].x = x;
            army2[index].y = y;
            battlefield[y][x] = &army2[index];
            index++;
        }
    }
}
void handle_player_turn(int player, UNIT* army, int army_size, UNIT* enemy_army, int enemy_size, int* surrendered_flag) {
    UNIT* selected_unit = NULL;
    int selected_x = 0, selected_y = 0;
    int width, height;
    getmaxyx(stdscr, height, width);

    // find the first alive unit from the current player's army
    for (int y = 0; y < FIELD_HEIGHT && !selected_unit; y++) {
        for (int x = 0; x < FIELD_WIDTH && !selected_unit; x++) {
            UNIT* u = battlefield[y][x];
            if (u && u->hp > 0 && is_unit_from_army(u, army, army_size)) {
                selected_unit = u;
                selected_x = x;
                selected_y = y;
            }
        }
    }
    int action_done = 0;
    // display current player's turn and instructions
    while (!action_done) {
        clear();
        mvprintw(1, (width - 15) / 2, "Player %d's turn", player);
        mvprintw(3, (width - 44) /2, "Use WASD to move, ENTER to move, F to attack");
        mvprintw(height - 2, (width - 20) /2, "Press X to surrender");
        mvprintw(height - 7, (width - 24) /2, "RED - ATTACKABLE TARGETS");
        mvprintw(height - 6, (width - 20) /2, "BLUE - SELECTED UNIT");
        mvprintw(height - 5, (width - 20) /2, "YELLOW - AOE TARGETS");

        // redraw the field and armies
        draw_field();
        display_armies(army_1, army1_size, army_2, army2_size);

        highlight_cell(selected_y, selected_x, cell_height, cell_width, start_y, start_x, 4); // highlight the selected cell (blue)
        highlight_attackable_targets(selected_unit, enemy_army, enemy_size, start_y, start_x); // highlight all attackable targets (red)
        // highlight the selected unit's cell so the player can see it clearly
        int cy = start_y + selected_y * cell_height + cell_height / 2;
        int cx = start_x + selected_x * cell_width + cell_width / 2;
        attron(A_REVERSE);
        mvaddch(cy, cx, mvinch(cy, cx) & A_CHARTEXT);
        attroff(A_REVERSE);

        refresh();

        int ch = getch(); //handle user input
        switch (ch) {
            case 'w': if (selected_y > 0) selected_y--; break;
            case 's': if (selected_y < FIELD_HEIGHT - 1) selected_y++; break;
            case 'a': if (selected_x > 0) selected_x--; break;
            case 'd': if (selected_x < FIELD_WIDTH - 1) selected_x++; break;
            // confirm movement with ENTER
            case '\n':
            case KEY_ENTER: {
                if (can_move_to(selected_unit, selected_x, selected_y)) {
                    int center = FIELD_WIDTH / 2;
                    if ((player == 1 && selected_x >= center) ||
                    (player == 2 && selected_x < center)) { // prevent crossing into enemy territory
                        break; // Invalid move 
                    }
                    // update battlefield state and unit position
                    battlefield[selected_unit->y][selected_unit->x] = NULL;
                    selected_unit->x = selected_x;
                    selected_unit->y = selected_y;
                    battlefield[selected_y][selected_x] = selected_unit;
                    action_done = 1; // end turn
                }
                break;
            }
            // attack with F
            case 'f':
            case 'F':
                attack_army(selected_unit, enemy_army, enemy_size);
                action_done = 1;
                break;
            // surrender with X
            case 'x':
            case 'X':
                *surrendered_flag = 1;
                return;
        }
    }
}
void handle_turn() {
    int turn = 0;
    int surrendered1 = 0, surrendered2 = 0;

    int height, width;
    getmaxyx(stdscr, height, width);

    while (1) {
        clear();
        // win or surrender
        if (surrendered1) {
            mvprintw(5, (width - 23) / 2 -1, "Army 1 has surrendered!");
            ascii_won_2();
            break;
        }
        if (surrendered2) {
            mvprintw(5, (width - 23) / 2 -1, "Army 2 has surrendered!");
            ascii_won_1();
            break;
        }
        if (check_victory(army_1, army1_size, army_2, army2_size)) break;

        mvprintw(0, width - 13, "Turn: %d", turn);
        refresh();
        // turns between players
        if (turn % 2 == 0) {
            handle_player_turn(1, army_1, army1_size, army_2, army2_size, &surrendered1);
        } else {
            handle_player_turn(2, army_2, army2_size, army_1, army1_size, &surrendered2);
        }
        turn++;
    }
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            battlefield[y][x] = NULL; // clear the battlefield
        }
    }

    mvprintw(height - 2, (width - 35) /2, "Game over. Press any key to exit...");
    refresh();
    getch();
    // clear the screen
    endwin();
    exit(0);
}
void attack_army(UNIT *attacker, UNIT *enemy_army, int size_b){
    int width, height;
    getmaxyx(stdscr, height, width);
    if (!attacker || attacker->hp <= 0) return; // skip if attacker is invalid or dead
    // highlight the attacking unit in blue
    highlight_cell(attacker->y, attacker->x, cell_height, cell_width, start_y, start_x, 2);
    refresh();

    const ITEM *items[2] = { attacker->item1, attacker->item2 };

    for (int i = 0; i < 2; i++) {
        const ITEM *item = items[i];
        if (!item) continue; // skip if no item in slot

        int range = item->range;
        int radius = item->radius;
        // loop over battlefield to find valid enemy targets in range
        for (int y2 = 0; y2 < FIELD_HEIGHT; y2++) {
            for (int x2 = 0; x2 < FIELD_WIDTH; x2++) {
                UNIT *target = battlefield[y2][x2];
                // check if this target is part of the enemy army
                int is_enemy = 0;
                for (int j = 0; j < size_b; j++) {
                    if (target == &enemy_army[j]) {
                        is_enemy = 1;
                        break;
                    }
                }
                if (!target || target->hp <= 0 || target == attacker || !is_enemy) // skip invalid, dead, non-enemy, or self targets
                    continue;
                // check if the enemy is in attack range
                int dist = abs(attacker->y - y2) + abs(attacker->x - x2);
                if (dist <= range) {
                    // highlight the target cell in red
                    highlight_cell(y2, x2, cell_height, cell_width, start_y, start_x, 1);
                    refresh();
                    napms(200); // pause for effect

                    // AOE: search enemy units in the radius
                    for (int yy = 0; yy < FIELD_HEIGHT; yy++) {
                        for (int xx = 0; xx < FIELD_WIDTH; xx++) {
                            int aoe_dist = abs(yy - y2) + abs(xx - x2);
                            if (aoe_dist > radius) continue; // outside of AOE radius

                            UNIT *aoe_target = battlefield[yy][xx];
                            int is_aoe_enemy = 0;
                            for (int k = 0; k < size_b; k++) {
                                if (aoe_target == &enemy_army[k]) {
                                    is_aoe_enemy = 1;
                                    break;
                                }
                            }

                            if (!aoe_target || aoe_target->hp <= 0 || !is_aoe_enemy)
                                continue;

                            // highlight AOE cell in yellow
                            highlight_cell(yy, xx, cell_height, cell_width, start_y, start_x, 3);
                            refresh();
                            napms(100);
                            // Calculate and apply damage
                            int total_def = 0;
                            if (aoe_target->item1) total_def += aoe_target->item1->def;
                            if (aoe_target->item2) total_def += aoe_target->item2->def;
                            int dmg = max(1, item->att - total_def);
                            aoe_target->hp -= dmg;
                            if (aoe_target->hp <= 0) { // if unit dies, remove it from battlefield
                                aoe_target->hp = 0;
                                battlefield[aoe_target->y][aoe_target->x] = NULL; // удалить с поля
                            }   

                            mvprintw(start_y + FIELD_HEIGHT * cell_height + 2, width / 2 - 20, 
                                     "%s hits %s with %s for %d damage!", 
                                     attacker->name, aoe_target->name, item->name, dmg);
                            refresh();
                            napms(400);
                        }
                    }
                }
            }
        }
    }
}
void handle_movement(UNIT *unit) {
        while (1) {
        int ch = getch();
        int dx = 0, dy = 0;

        if (ch == 'w') dy = -1;
        else if (ch == 's') dy = 1;
        else if (ch == 'a') dx = -1;
        else if (ch == 'd') dx = 1;

        if (dx != 0 || dy != 0) {
            move_unit(unit, dx, dy);  // move instantly
        }
        draw_field(); // redraw field after every move
    }
}
void move_unit(UNIT *unit, int dx, int dy) {
    if (dx < -1 || dx > 1 || dy < -1 || dy > 1) {
        return; // invalid move
    }

    int new_x = unit->x + dx;
    int new_y = unit->y + dy;

    if (new_x >= 0 && new_x < FIELD_WIDTH && 
        new_y >= 0 && new_y < FIELD_HEIGHT && // check if the new position is within bounds and empty
        battlefield[new_y][new_x] == NULL) {

        battlefield[unit->y][unit->x] = NULL; // remove unit from current position
        // move unit to new position
        battlefield[new_y][new_x] = unit; 
        unit->x = new_x;
        unit->y = new_y;
        draw_field();
        refresh();
    }
}
bool check_victory(UNIT army1[], int size1, UNIT army2[], int size2) {
    bool army1_alive = false;
    bool army2_alive = false;

    for (int i = 0; i < size1; i++) { //checking if army1 has any alive units
        if (army1[i].hp > 0) {
            army1_alive = true;
            break;
        }
    }

    for (int i = 0; i < size2; i++) { //checking if army2 has any alive units
        if (army2[i].hp > 0) {
            army2_alive = true;
            break;
        }
    }

    if (!army1_alive) { // if army1 is dead
        ascii_won_2();
        return true;
    } else if (!army2_alive) { 
        ascii_won_1();
        return true;
    }
    return false;
}
void ascii_won_1(){
    const char *ascii_art[] = {
        "    _                              _     __        ___           _ ",
        "   / \\   _ __ _ __ ___  _   _     / |    \\ \\      / (_)_ __  ___| |",
        "  / _ \\ | '__| '_ ` _ \\| | | |    | |     \\ \\ /\\ / /| | '_ \\/ __| |",
        " / ___ \\| |  | | | | | | |_| |    | |      \\ V  V / | | | | \\__ \\_|",
        "/_/   \\_\\_|  |_| |_| |_|\\__, |    |_|       \\_/\\_/  |_|_| |_|___(_)",
        "                        |___/                                      ",
    };
    int ascii_lines = 6; // number of lines in the ascii art
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
void ascii_won_2(){
    const char *ascii_art[] = {
        "    _                              ____      __        ___           _ ",
        "   / \\   _ __ _ __ ___  _   _     |___ \\     \\ \\      / (_)_ __  ___| |",
        "  / _ \\ | '__| '_ ` _ \\| | | |      __) |     \\ \\ /\\ / /| | '_ \\/ __| |",
        " / ___ \\| |  | | | | | | |_| |     / __/       \\ V  V / | | | | \\__ \\_|",
        "/_/   \\_\\_|  |_| |_| |_|\\__, |    |_____|       \\_/\\_/  |_|_| |_|___(_)",
        "                        |___/                                          ",
    };
    int ascii_lines = 6; // number of lines in the ascii art
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