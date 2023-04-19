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
    on_press = MenuOnPress;
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
    on_press = ShuffleOnPress;
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
    prev_state = -1;
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

void blackjack_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    on_press = BlackjackOnPress;
    curr_state = BlackJack;
    char title[] = "Pick Selection Mode";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"Winner Select","Hand Select"};
    screen new_states[MAXSELECTIONS] = {WinnerSelect, NumPlayerSelect};
    num_selections = 2;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = CardShuffling;
}

void numplayer_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    on_press = NumPlayerOnPress;
    curr_state = NumPlayerSelect;
    char title[] = "Pick Number of Players";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"1 Player","2 Players","3 Players","4 Players"};
    screen new_states[MAXSELECTIONS] = {PlayerSelect, PlayerSelect, PlayerSelect, PlayerSelect};
    num_selections = 4;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = BlackJack;
}

void playerselect_state() {
    needs_reset = 1;
    draw_screen = DrawMenuScreen;
    on_press = PlayerSelectOnPress;
    curr_state = PlayerSelect;
    char title[] = "Pick a Hand To Alter";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"Player 1","Player 2","Player 3","Player 4"};
    screen new_states[MAXSELECTIONS] = {ValueSelect, ValueSelect, ValueSelect, ValueSelect};
    num_selections = 4;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = NumPlayerSelect;
}

void valueselect_state() {
    needs_reset = 1;
    draw_screen = DrawValueSelect;
    on_press = ValueSelectOnPress;
    curr_state = ValueSelect;
    char title[] = "Select Card Value";
    strcpy(screen_title, title);
    curr_selection = 0;
    row = 0;
    col = 0;
    prev_state = -1;
}

void suitselect_state() {
    needs_reset = 1;
    draw_screen = DrawSuitSelect;
    on_press = SuitSelectOnPress;
    curr_state = SuitSelect;
    char title[] = "Select Card Value";
    strcpy(screen_title, title);
    curr_selection = 0;
    row = 0;
    col = 0;
    prev_state = -1;
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
    } else if(state == Random) {

    } else if(state == BlackJack) {
        blackjack_state();
    } else if(state == NumPlayerSelect) {
        numplayer_state();
    } else if(state == PlayerSelect) {
        playerselect_state();
    }  else if(state == ValueSelect) {
        valueselect_state();
    } else if(state == SuitSelect) {
        suitselect_state();
    } else if(state == WinnerSelect) {
        //winnerselect_state();
    } else if(state == Confirm) {
        //confirm_state();
    }  else if(state == Loading) {
        //loading_state();
    }
}

#endif /* SCREEN_C_ */
