#ifndef LOGIC_H
#define LOGIC_H
#include "data.h"

#define FIELD_WIDTH 8
#define FIELD_HEIGHT 5

extern UNIT *battlefield[FIELD_HEIGHT][FIELD_WIDTH]; // battlefield is a 2D array of pointers to UNIT
extern UNIT *army_1;
extern UNIT *army_2;
extern int army1_size;
extern int army2_size;

int max(int a, int b); //searching for the maximum
void init_armies();
int select_number_of_units(int army_num);
void set_unit_name(UNIT *unit);
void select_weapons(UNIT *unit);
void auto_place_units(UNIT army1[], int size1, UNIT army2[], int size2);
void move_unit(UNIT *unit, int new_y, int new_x);
bool check_victory(UNIT army1[], int size1, UNIT army2[], int size2);
void handle_turn();
void attack_army(UNIT *attacker, UNIT *enemy_army, int size_b);
bool is_unit_from_army(UNIT *unit, UNIT *army, int size);

#endif //LOGIC_H
