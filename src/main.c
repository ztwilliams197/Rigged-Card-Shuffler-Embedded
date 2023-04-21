/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "stm32f0xx.h"
#include "init.h"
#include "lcd.h"
#include "Screen.h"
#include "tty.h"
#include "StepperMotor.h"
#include "structs.h"
#include "uart.h"

int draw_color = 0xffff;
int needs_reset = 1;

char s1[20];
char s2[20];
char s3[25];

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

void gen_N_pulses_dispenser(int n) {
    TIM15->RCR = n - 1;
    TIM15->CR1 |= TIM_CR1_CEN;
}

void gen_N_pulses_wheel(int n) {
    TIM16->RCR = n - 1;
    TIM16->CR1 |= TIM_CR1_CEN;
}

// End Motor Control

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
    gen_N_pulses_dispenser(numStepsBetweenBins((current_bin + 26) % NUM_BINS, 1));
    if(EXTI->PR & EXTI_PR_PR4) { // Up -- 1
        on_press(0);
//        if(curr_selection > 0) {
        //    curr_selection -= 1;
        //    needs_reset = 1;
        //}
    	toggle_heartbeat_led();
    	set_gpiob(3, heartbeat);
        EXTI->PR |= EXTI_PR_PR4;
    }
    else if(EXTI->PR & EXTI_PR_PR5) { // Down -- 4
        on_press(1);
        //if(prev_state != -1) {
        //    change_state(prev_state);
        //}
    	toggle_heartbeat_led();
		set_gpiob(4, heartbeat);
        EXTI->PR |= EXTI_PR_PR5;
    }
    else if(EXTI->PR & EXTI_PR_PR10) { // Right -- 5
        on_press(2);
        //change_state(selection_states[curr_selection]);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR10;
    }
    else if(EXTI->PR & EXTI_PR_PR11) { // Left -- 3
        on_press(3);
//        gen_N_pulses(200);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR11;
    }
    else if(EXTI->PR & EXTI_PR_PR12) { // Enter -- 2
        on_press(4);
        //if(curr_selection < num_selections - 1) {
        //    curr_selection += 1;
        //    needs_reset = 1;
        //}
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR12;
    }
}

// End EXTI

void write_string(char *c, int count) {
    c[count + 1] = '\0';
    send_string(USART1, c);
}

void USART1_IRQHandler() {
//    USART1->ISR &= ~USART_ISR_RXNE;
	int packet_int = USART1->RDR & 0xff; // reading RDR should clear RXNE flag (line above)
	rx_packet packet = translate_packet(packet_int);

    // debug print to LCD screen
	switch (packet.action) {
		case RX_RESET:
			addInputToBuffer("rx RESET");
			break;
		case RX_START_SHUFFLE_MCU:
			addInputToBuffer("rx MCU shf ACK");
			break;
		case RX_START_SHUFFLE_SBC:
			addInputToBuffer("rx SBC shf ACK");
			break;
		case IDENTIFY_SLOT:
			//char s1[20] = "rx Slot ID = ~~";
		    strcpy(s1, "rx Slot ID = ~~");
			s1[13] = '0' + (packet.metadata / 10) % 10;
			s1[14] = '0' + packet.metadata % 10;
			addInputToBuffer(s1);
			break;
		case REINDEX_SLOT:
			//char s2[20] = "rx ReIndex = ~~";
		    strcpy(s2, "rx ReIndex = ~~");
			s2[13] = '0' + (packet.metadata / 10) % 10;
			s2[14] = '0' + packet.metadata % 10;
			addInputToBuffer(s2);
			break;
		default:
			//char s3[25] = "Unknown packet: 0x~~";
		    strcpy(s3, "Unknown packet: 0x~~");
			int h = packet_int >> 4;
			s3[18] = (h >= 10 ? 'a' - 10 : '0') + h;
			h = packet_int & 0xf;
			s3[19] = (h >= 10 ? 'a' - 10 : '0') + h;
			addInputToBuffer(s3);
			break;
	}
}

/*
void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;

}*/

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
	//change_state(PrepScreen);
    change_state(CardShuffling);

    //addInputToBuffer("Hello World");
	int i = 0;
	for(;;) {
	    // Wait for ack

	    change_state(CardShuffling);

	    //while(curr_state != Shuffle) {
	        if(needs_reset) {
	                    draw_screen();
	                    needs_reset = 0;
	        }
	    //}

	    //change_state(Loading);

	    // Send Shuffle info

	    /*for(int i = 0; i < 52; i++) {
	     * wait for bin number
	     * move motors
	     * send signal to pi that we moved motors
	    }
	     */

		//sleep_micros(1670000);
		//if (++i % 100 == 0)
			//toggle_heartbeat_led();
	    //sleep_ms(1000);
//		if (heartbeat)
//		pulse_stepper();
		//gen_N_pulses(50);
	}
}
