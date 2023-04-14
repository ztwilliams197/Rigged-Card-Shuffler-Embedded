/*
 * Screen.h
 *
 *  Created on: Feb 10, 2023
 *      Author: ztwil
 */

#ifndef SCREEN_H_
#define SCREEN_H_

#define MAXSELECTIONS 17

typedef enum screens{Select,Testing,CardShuffling,TestLCD,TestMotors,TestUART,BlackJack,Random}screen;

int needs_reset;
void (*draw_screen)();
screen curr_state;
char screen_title[20];
char selections[MAXSELECTIONS][20];
screen selection_states[MAXSELECTIONS];
int num_selections;
int curr_selection;
screen prev_state;

void DrawMenuScreen();
void change_state(screen state);
void select_state();
void testing_state();
void cardshuffling_state();
void DrawUARTInput();
void test_UART_state();
void addInputToBuffer(const char string[]);

#endif /* SCREEN_H_ */
