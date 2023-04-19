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
    screen game;
    int num_players;
    int mode;
    int winners[MAX_PLAYERS];
    int hands[MAX_PLAYERS][5];
    int curr_player;
} shuffle_data;

#endif /* STRUCTS_H_ */
