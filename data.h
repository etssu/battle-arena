#ifndef DATA_H
#define DATA_H

#define NUMBER_OF_ITEMS 16  // velkost databazy predmetov
#define MAX_NAME 100        // maximalna dlzka nazvu/mena
#define MIN_ARMY 1          // minimalny pocet bojovych jednotiek v armade
#define MAX_ARMY 5          // maximalny pocet bojovych jednotiek v armade

// Struktura pre predmet v inventari
typedef struct item {
    char name[MAX_NAME + 1];        // nazov predmetu
    int att;                        // utok
    int def;                        // obrana
    int slots;                      // pocet slotov, ktore zabera predmet v inventari
    int range;                      // utocny dosah/dostrel predmetu
    int radius;                     // sirka zasahu (alebo damage radius)
} ITEM;

// Struktura pre bojovu jednotku
typedef struct unit {
    char name[MAX_NAME + 1];    // meno jednotky
    const ITEM *item1;          // prvy predmet v inventari (ukazuje do pola 'items')
    const ITEM *item2;          // druhy predmet v inventari (ukazuje do pola 'items')
    int hp;                     // zdravie (z angl. hit points)

    int x;  // position
    int y;
} UNIT;

// databaza predmetov, ktore moze mat bojova jednotka v inventari
extern const ITEM items[NUMBER_OF_ITEMS];

#endif //DATA_H