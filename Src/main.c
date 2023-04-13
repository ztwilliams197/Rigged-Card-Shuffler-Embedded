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
#include <string.h>
#include <stdio.h>
#include <unistd.h>

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

// end screen

void pulse_stepper() {
	set_gpioa(2, 1);
	set_gpioa(6, 1);
	sleep_micros(1000);
	set_gpioa(2, 0);
	set_gpioa(6, 0);
	sleep_micros(1000);
}

// Begin GPIO Exti stuff

void EXTI4_15_IRQHandler(void) {
	pulse_stepper();
    if(EXTI->PR & EXTI_PR_PR4) { // Up -- 1
//        if(curr_selection > 0) {
//            curr_selection -= 1;
//            needs_reset = 1;
//        }
    	toggle_heartbeat_led();
    	set_gpiob(3, heartbeat);
        EXTI->PR |= EXTI_PR_PR4;
    }
    else if(EXTI->PR & EXTI_PR_PR5) { // Left -- 4
//        if(prev_state != -1) {
//            change_state(prev_state);
//        }
    	toggle_heartbeat_led();
		set_gpiob(4, heartbeat);
        EXTI->PR |= EXTI_PR_PR5;
    }
    else if(EXTI->PR & EXTI_PR_PR10) { // Select -- 5
//        change_state(selection_states[curr_selection]);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR10;
    }
    else if(EXTI->PR & EXTI_PR_PR11) { // Right -- 3
//        gen_N_pulses(200);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR11;
    }
    else if(EXTI->PR & EXTI_PR_PR12) { // Down -- 2
//        if(curr_selection < num_selections - 1) {
//            curr_selection += 1;
//            needs_reset = 1;
//        }
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR12;
    }
}

// End EXTI

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

	gen_N_pulses(100);


//    init_system();
//    change_state(Select);
	for(;;) {
		sleep_ms(167);
//		toggle_heartbeat_led();
//		needs_reset = 1;
//	    draw_screen();
//		if (heartbeat)
//		pulse_stepper();
		gen_N_pulses(100);
	}
}
