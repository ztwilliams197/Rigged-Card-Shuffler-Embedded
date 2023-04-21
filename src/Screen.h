/*
 * Screen.h
 *
 *  Created on: Feb 10, 2023
 *      Author: ztwil
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#define MAXSELECTIONS 17

typedef enum screens{Void,CardShuffling,NumPlayerSelect,WinnerSelect,Confirm,Loading,WakeSyncScreen,Shuffling}screen;

int needs_reset;
void (*draw_screen)();
void (*on_press)(int);
screen curr_state;
char screen_title[30];
char selections[MAXSELECTIONS][30];
screen selection_states[MAXSELECTIONS];
int num_selections;
int curr_selection;
screen prev_state;

int row;
int col;

void DrawMenuScreen();
void change_state(screen state);
void cardshuffling_state();
void MenuOnPress(int);
void ShuffleOnPress(int);
void NumPlayerOnPress(int);
void WinnerSelectOnPress(int);
void NothingOnPress(int);
void addInputToBuffer(const char string[]);

#endif /* SCREEN_H_ */
