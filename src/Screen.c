/*
 * Screen.c
 *
 *  Created on: Feb 10, 2023
 *      Author: ztwil
 */

#ifndef SCREEN_C_
#define SCREEN_C_

#include <unistd.h>

#include "Screen.h"
#include "structs.h"
#include "lcd.h"
#include <string.h>

void cardshuffling_state() {
    draw_screen = DrawMenuScreen;
    on_press = ShuffleOnPress;
    curr_state = CardShuffling;
    char title[] = "Select Game";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"BlackJack","Random"};
    screen new_states[MAXSELECTIONS] = {NumPlayerSelect, Confirm};
    num_selections = 2;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = -1;
    needs_reset = 1;
}

void numplayer_state() {
    draw_screen = DrawMenuScreen;
    on_press = NumPlayerOnPress;
    curr_state = NumPlayerSelect;
    char title[] = "Pick Number of Players";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"1 Player","2 Players","3 Players","4 Players"};
    screen new_states[MAXSELECTIONS] = {WinnerSelect, WinnerSelect, WinnerSelect, WinnerSelect};
    num_selections = 4;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = CardShuffling;
    needs_reset = 1;
}

void winnerselect_state() {
    shuffle.dealer = 0;
    shuffle.table = 0;
    for(int i = 0; i < MAX_PLAYERS; i++) {
        shuffle.winners[i] = 0;
    }

    draw_screen = DrawMenuScreen;
    on_press = WinnerSelectOnPress;
    curr_state = WinnerSelect;
    char title[] = "Pick Winners";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"Start Deal","Dealer","Table","Player 1","Player 2", "Player 3", "Player 4"};
    screen new_states[MAXSELECTIONS] = {Confirm, Confirm, Confirm, WinnerSelect, WinnerSelect, WinnerSelect, WinnerSelect};
    num_selections = shuffle.num_players+3;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    prev_state = NumPlayerSelect;
    needs_reset = 1;
}

void confirm_state() {
    prev_state = curr_state;
    draw_screen = DrawMenuScreen;
    on_press = MenuOnPress;
    curr_state = Confirm;
    char title[] = "Start Deal?";
    strcpy(screen_title, title);
    char new_selections[MAXSELECTIONS][20] = {"Confirm"};
    screen new_states[MAXSELECTIONS] = {Shuffling};
    num_selections = 1;
    for(int i = 0; i < num_selections; i++) {
        strcpy(selections[i],new_selections[i]);
        selection_states[i] = new_states[i];
    }
    curr_selection = 0;
    needs_reset = 1;
}

void loading_state() {
    draw_screen = DrawMenuScreen;
    on_press = NothingOnPress;
    curr_state = Loading;
    char title[] = "Loading";
    strcpy(screen_title, title);
    num_selections = 0;
    curr_selection = 0;
    prev_state = -1;
    needs_reset = 1;
}

void wakesync_state() {
    draw_screen = DrawMenuScreen;
    on_press = NothingOnPress;
    curr_state = WakeSyncScreen;
    char title[] = "Syncing with Pi";
    strcpy(screen_title, title);
    num_selections = 0;
    curr_selection = 0;
    prev_state = -1;
    needs_reset = 1;
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
    if(state == curr_state) {
        return;
    }

    if(state == CardShuffling) {
        cardshuffling_state();
    } else if(state == NumPlayerSelect) {
        numplayer_state();
    } else if(state == WinnerSelect) {
        winnerselect_state();
    } else if(state == Confirm) {
        confirm_state();
    }  else if(state == Loading) {
        loading_state();
    }
}

// ON PRESS START

void change_selection(int num) {
    if(num < 0 && curr_selection + num >= 0) {
        curr_selection += num;
        needs_reset = 1;
    } else if(num > 0 && curr_selection + num <= num_selections - 1) {
        curr_selection += num;
        needs_reset = 1;
    }
}

void move_back() {
    if(prev_state != -1) {
        change_state(prev_state);
    }
}

void move_forward() {
    change_state(selection_states[curr_selection]);
}

void MenuOnPress(int input) { // 0 = Up 1 = Down 2 = Right 3 = Left 4 = Select
    switch (input) {
        case 0:
            change_selection(-1);
            break;
        case 1:
            change_selection(1);
            break;
        case 2:
            move_forward();
            break;
        case 3:
            move_back();
            break;
        case 4:
            move_forward();
            break;
    }
}

void NothingOnPress(int input) {
    return;
}

void ShuffleOnPress(int input) {
    if(input == 4) {
        strcpy(shuffle.game, selections[curr_selection]);
    }
    MenuOnPress(input);
}

void NumPlayerOnPress(int input) {
    if(input == 4) {
        shuffle.num_players = curr_selection + 1;
    }
    MenuOnPress(input);
}

void WinnerSelectOnPress(int input) {
    if(input == 4) {
        if(!strcmp(selections[curr_selection],"Start Deal") && (shuffle.winners[0] == 1 || shuffle.winners[1] == 1 || shuffle.winners[2] == 1 || shuffle.winners[3] == 1)) {
            MenuOnPress(input);
        } else if(!strcmp(selections[curr_selection],"Dealer")) {
            shuffle.dealer = 1;
            MenuOnPress(input);
        } else if(!strcmp(selections[curr_selection],"Table")) {
            shuffle.table = 1;
            MenuOnPress(input);
        } else {
            shuffle.winners[curr_selection - 3] = 1;
        }
    } else {
        MenuOnPress(input);
    }
}

// ON PRESS END

// DRAW START

void DrawMenuScreen() {
    if(needs_reset) {
        LCD_Clear(0x0000);
        needs_reset = 0;
    }

    //Draw Title
    LCD_DrawString(0,00,0xffff,0x0,screen_title,16,1);

    //Draw Selections
    for(int i = curr_selection; i < num_selections; i++) {
        if(i == curr_selection) {
            LCD_DrawFillRectangle(0,(i-curr_selection)*18 + 20,220,12 + (i-curr_selection)*18 + 20 ,0xffff);
            LCD_DrawString(0,(i-curr_selection) * 18 + 20, 0x0, 0x0, selections[i], 12, 1);
        } else {
            LCD_DrawString(0,(i-curr_selection) * 18 + 20, 0xffff, 0x0, selections[i], 12, 1);
        }
    }
}

// DRAW END

#endif /* SCREEN_C_ */
