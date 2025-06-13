#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "data.h"
extern int cell_height;
extern int cell_width;
extern int field_height;
extern int field_width;
extern int start_y;
extern int start_x;

void init_menu_art();
int init_menu_buttons();

void display_armies(UNIT army1[], int size1, UNIT army2[], int size2);
void draw_field();
void rules();
void highlight_cell(int y, int x, int cell_height, int cell_width, int start_y, int start_x, int color_pair);
void highlight_attackable_targets(UNIT *unit, UNIT *enemy_army, int enemy_size, int start_y, int start_x);
int show_action_menu(UNIT *unit);
#endif //GRAPHICS_H
