/*
 * structs.h
 *
 *  Created on: Apr 19, 2023
 *      Author: ztwil
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_
#include "Screen.h"

#define MAX_PLAYERS 4

typedef struct _shuffle_data {
    char game[30];
    int num_players;
    int dealer;
    int table;
    int winners[MAX_PLAYERS];
} shuffle_data;

shuffle_data shuffle;

#endif /* STRUCTS_H_ */
