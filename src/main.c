/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "init.h"
#include "lcd.h"
#include "Screen.h"
#include "tty.h"
#include "StepperMotor.h"
#include "structs.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

shuffle_data shuffle;
char* card_vals[4][4] = {{"A", "K", "Q", "J"},{"10", "9", "8", "7"},{"6","5","4","3"},{"2","","",""}};
char* suit_vals[2][2] = {{"Diamond", "Spade"},{"Heart", "Clubs"}};

int draw_color = 0xffff;
int needs_reset = 1;

// Weird stuff
void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void sleep_ms(int ms) {
    nano_wait(ms * 1000000);
}

void sleep_micros(int mus) {
    nano_wait(mus * 1000);
}

int heartbeat = 0;

void set_gpioa(int pin, int enable) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~(0x11 << (2 * pin));
	GPIOA->MODER |= 0x01 << (2 * pin);
	GPIOA->BSRR = 1 << (enable ? pin : (16 + pin));
}


void set_gpiob(int pin, int enable) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(0x11 << (2 * pin));
	GPIOB->MODER |= 0x01 << (2 * pin);
	GPIOB->BSRR = 1 << (enable ? pin : (16 + pin));
}

void set_heartbeat_led(int enable) {
	set_gpiob(1, enable);
	heartbeat = enable;
}

void toggle_heartbeat_led() {
	set_heartbeat_led(heartbeat ? 0 : 1);
}

int __io_putchar(int c)
{
    while(!(USART5->ISR & USART_ISR_TXE));
    if(c == '\n')
    {
        USART5->TDR = '\r';
        while(!(USART5->ISR & USART_ISR_TXE));
    }
    USART5->TDR = c;
    return c;
}

void init_system() {
    LCD_Setup();
    init_buttons();
    init_opm_tim();
}

// Begin Motor Control

int high = 0;

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;
    if(high) {
        GPIOA->BSRR |= GPIO_BSRR_BR_8;
        high = 0;
    } else {
        GPIOA->BSRR |= GPIO_BSRR_BS_8;
        high = 1;
    }
}

void gen_N_pulses(int n) {
    TIM15->RCR = n - 1;
    TIM15->CR1 |= TIM_CR1_CEN;

    TIM16->RCR = n - 1;
	TIM16->CR1 |= TIM_CR1_CEN;
}

// End Motor Control

// Begin Screen

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

void MenuOnPress(int input) { // 0 = Up 1 = Down 2 = Left 3 = Right 4 = Select
}

void ShuffleOnPress(int input) {
    if(input == 4) {
        shuffle.game = selection_states[curr_selection];
    }
}

void BlackjackOnPress(int input) { // 0 = WinnerSelect 1 = HandSelect
    if(input == 4) {
        if(selection_states[curr_selection] == WinnerSelect) {
            shuffle.mode = 0;
        } else {
            shuffle.mode = 1;
        }
    }
}

void NumPlayerOnPress(int input) {
    if(input == 4) {
        shuffle.num_players = curr_selection + 1;
    }
}

void PlayerSelectOnPress(int input) {
    if(input == 4) {
        shuffle.curr_player = curr_selection + 1;
    }
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

// end screen

void pulse_stepper(int n) {
    int i;
    for(i = 0; i < n; i++) {
        set_gpioa(2, 1);
        set_gpioa(6, 1);
        sleep_micros(1000);
        set_gpioa(2, 0);
        set_gpioa(6, 0);
        sleep_micros(1000);
    }
}

// Begin GPIO Exti stuff

void EXTI4_15_IRQHandler(void) {
    //gen_N_pulses(numStepsBetweenBins((current_bin + 1) % NUM_BINS, 1));
    if(EXTI->PR & EXTI_PR_PR4) { // Up -- 1
        on_press(0);
        if(curr_selection > 0) {
            curr_selection -= 1;
            needs_reset = 1;
        }
    	toggle_heartbeat_led();
    	set_gpiob(3, heartbeat);
        EXTI->PR |= EXTI_PR_PR4;
    }
    else if(EXTI->PR & EXTI_PR_PR5) { // Left -- 4
        on_press(2);
        if(prev_state != -1) {
            change_state(prev_state);
        }
    	toggle_heartbeat_led();
		set_gpiob(4, heartbeat);
        EXTI->PR |= EXTI_PR_PR5;
    }
    else if(EXTI->PR & EXTI_PR_PR10) { // Select -- 5
        on_press(4);
        change_state(selection_states[curr_selection]);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR10;
    }
    else if(EXTI->PR & EXTI_PR_PR11) { // Right -- 3
        on_press(3);
//        gen_N_pulses(200);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR11;
    }
    else if(EXTI->PR & EXTI_PR_PR12) { // Down -- 2
        on_press(1);
        if(curr_selection < num_selections - 1) {
            curr_selection += 1;
            needs_reset = 1;
        }
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR12;
    }
}

// End EXTI

void write_char(char c) {
    while(!(USART1->ISR & USART_ISR_TXE)) { }
    USART1->TDR = c;
}

void write_string(char* c, int count) {
    for(int i = 0; i < count; i++){
        write_char(c[i]);
    }
}

void USART1_IRQHandler() {
    USART1->ISR &= ~USART_ISR_RXNE;
    unsigned char value = USART1->RDR; // should clear rxne flag
    char str[50] = "Received Byte: 0x~~\n";
    str[17] = (value >> 4) + '0';
    str[18] = (value & 0xf) + '0';
    write_string(str, strlen(str));

    // TODO pass this as a hex string to zach's shit
}

int main(void)
{
	init_buttons();
	LCD_Setup();

	init_opm_tim();

	// dir
	set_gpioa(4, 1);
	set_gpioa(11, 1);
	// ~en
	set_gpioa(5, 0);
	set_gpioa(12, 0);

	sleep_ms(10);

	//gen_N_pulses(100);


//    init_system();
    change_state(ValueSelect);

    //addInputToBuffer("Hello World");
	int i = 0;
	for(;;) {
//		sleep_micros(1670);
		if (++i % 100 == 0)
			toggle_heartbeat_led();
	    draw_screen();
	    //sleep_ms(1000);
//		if (heartbeat)
//		pulse_stepper();
		//gen_N_pulses(100);
	}
}
