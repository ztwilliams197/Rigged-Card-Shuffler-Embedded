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

char* card_vals[4][4] = {{"A", "K", "Q", "J"},{"10", "9", "8", "7"},{"6","5","4","3"},{"2","","",""}};
char* suit_vals[2][2] = {{"Diamond", "Spade"},{"Heart", "Clubs"}};

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
    needs_reset = 1;
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

void ShuffleOnPress(int input) {
    if(input == 4) {
        shuffle.game = selection_states[curr_selection];
    }
    MenuOnPress(input);
}

void BlackjackOnPress(int input) { // 0 = WinnerSelect 1 = HandSelect
    if(input == 4) {
        if(selection_states[curr_selection] == WinnerSelect) {
            shuffle.mode = 0;
        } else {
            shuffle.mode = 1;
        }
    }
    MenuOnPress(input);
}

void NumPlayerOnPress(int input) {
    if(input == 4) {
        shuffle.num_players = curr_selection + 1;
    }
    MenuOnPress(input);
}

void PlayerSelectOnPress(int input) {
    if(input == 4) {
        shuffle.curr_player = curr_selection + 1;
    }
    MenuOnPress(input);
}

void ValueSelectOnPress(int input) {
    needs_reset = 1;
    if(input == 0) {
        if(col > 0) {
            col--;
        }
    } else if(input == 1) {
        if(col < 3) {
            col++;
        }
    } else if(input == 2) {
        if(row > 0) {
            row--;
        }
    } else if(input == 3) {
        if(row < 3) {
            row++;
        }
    }
}

void SuitSelectOnPress(int input) {
    needs_reset = 1;
    if(input == 0) {
        if(col > 0) {
            col--;
        }
    } else if(input == 1) {
        if(col < 1) {
            col++;
        }
    } else if(input == 2) {
        if(row > 0) {
            row--;
        }
    } else if(input == 3) {
        if(row < 1) {
            row++;
        }
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

void DrawValueSelect() {
    if(needs_reset) {
        LCD_Clear(0x0000);
        needs_reset = 0;
    }

    LCD_DrawString(50,00,0xffff,0x0,screen_title,16,1);

    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            LCD_DrawString(46*(i+1),64*(k+1),0xffff, 0x0, card_vals[k][i], 16,1);
            if(k == col && i == row) {
                LCD_DrawRectangle((46*(i+1))-5, (64*(k+1))-5, (46*(i+1))+20, (64*(k+1))+20, 0xffff);
            }
        }
    }
}

void DrawSuitSelect() {
    if(needs_reset) {
        LCD_Clear(0x0000);
        needs_reset = 0;
    }
    LCD_DrawString(40,0,0xffff,0x0,"Pick The Suit You Want",16,1);
    //Draw Diamond 320,240
    u16 x = 60;
    u16 y = 90;
    u16 m = 30;
    LCD_DrawFillTriangle(x-m,y,x+m,y,x,y-m*1.73,0xf8a2);//up
    LCD_DrawFillTriangle(x-m,y,x+m,y,x,y+m*1.73,0xf8a2);//down

    //Draw Heart
    x = 60;
    y = 230;
    m = 20;

    LCD_Circle(x-m,y,m,1,0xF8A2);
    LCD_Circle(x+m,y,m,1,0xF8A2);
    LCD_DrawFillTriangle(x-37, y+10, x+37, y+10, x, y+60,0xF8A2);
    LCD_Circle(x,y+10,5,1,0xF8A2);

    //Draw Spade
    x = 180;
    y = 100;
    m = 20;

    LCD_Circle(x-m,y,m,1,0xffff);
    LCD_Circle(x+m,y,m,1,0xffff);
    LCD_DrawFillTriangle(x-m*1.866, y-m/2, x+m*1.866, y-m/2, x, y-2*m*1.866*.8,0xffff);
    LCD_DrawFillTriangle(x,y,x-m*.5,y+m*1.5,x+m*.5,y+m*1.5,0xffff);
    LCD_Circle(x,y-m/2,5,1,0xffff);

    //Draw Clubs
    x = 180;
    y = 250;
    m = 40;

    LCD_Circle(x,y,m*.3,1,0xffff);
    LCD_Circle(x,y-m*.6,m*.45,1,0xffff);
    LCD_Circle(x-m*.6,y+m*.15,m*.45,1,0xffff);
    LCD_Circle(x+m*.6,y+m*.15,m*.45,1,0xffff);
    LCD_DrawFillTriangle(x,y,x-m/6,y+m*.8,x+m/6,y+m*.8,0xffff);

    for(int i = 0; i < 4; i++) {
        for(int k = 0; k < 4; k++) {
            if(i == col && k == row) {
                LCD_DrawRectangle(10+(k*120),30+(i*160),110+(k*120),150+(i*160),0xffff);
            }
        }
    }
}

// DRAW END

void DrawUARTInput() {
    if(needs_reset) {
        LCD_Clear(0x0000);
        needs_reset = 0;
    }

    //Draw Title
    LCD_DrawString(0,00,0xffff,0x0,screen_title,16,1);

    //Draw Selections
    for(int i = 0; i < num_selections; i++) {
        LCD_DrawString(0,(i) * 18 + 20, 0xffff, 0x0, selections[i], 12, 1);
    }
}

#endif /* SCREEN_C_ */
