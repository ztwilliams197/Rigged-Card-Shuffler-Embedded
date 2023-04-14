/*
 * Screen.c
 *
 *  Created on: Feb 10, 2023
 *      Author: ztwil
 */

#ifndef SCREEN_C_
#define SCREEN_C_

#include "Screen.h"
#include <string.h>

void select_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    curr_state = Select;
    char title[] = "Select Screen";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"Testing","CardShuffling"};
    screen new_states[MAXSELECTIONS] = {Testing, CardShuffling};
    num_selections = 2;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = -1;
}

void testing_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    curr_state = Testing;
    char title[] = "Select Test";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"TestLCD","TestMotors"};
    screen new_states[MAXSELECTIONS] = {TestLCD, TestMotors};
    num_selections = 2;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = Select;
}

void cardshuffling_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    curr_state = CardShuffling;
    char title[] = "Select Game";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"BlackJack","Random"};
    screen new_states[MAXSELECTIONS] = {BlackJack, Random};
    num_selections = 2;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = Select;
}

void test_UART_state() {
    needs_reset = 1;
    draw_screen = DrawUARTInput;
    curr_state = TestUART;
    char title[] = "Testing UART";
    strcpy(screen_title, title);
    num_selections = 0;
    curr_selection = 0;
    prev_state = Testing;
}

void addInputToBuffer(const char string[]) {
    strcpy(selections[curr_selection], string);
    curr_selection = (curr_selection + 1) % MAXSELECTIONS;
    if(num_selections < MAXSELECTIONS) {
        num_selections += 1;
    }
    needs_reset = 1;
}

void change_state(screen state) {
    if(state == Select) {
        select_state();
    } else if(state == Testing) {
        testing_state();
    } else if(state == CardShuffling) {
        cardshuffling_state();
    } else if(state == TestLCD) {

    } else if(state == TestMotors) {

    } else if(state == TestUART) {
        test_UART_state();
    }
}

#endif /* SCREEN_C_ */
