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

int got_reset = 0;
int got_mcu_start = 0;
int got_sbc_start = 0;
int current_bin_count = 0;

void USART1_IRQHandler() {
//    USART1->ISR &= ~USART_ISR_RXNE;
	int packet_int = USART1->RDR & 0xff; // reading RDR should clear RXNE flag (line above)
	rx_packet packet = translate_packet(packet_int);

	switch (packet.action) {
		case RX_RESET:
			got_reset = 1;
//			addInputToBuffer("rx RESET");
			break;
		case RX_START_SHUFFLE_MCU:
			got_mcu_start = 1;
//			addInputToBuffer("rx MCU shf ACK");
			break;
		case RX_START_SHUFFLE_SBC:
			got_sbc_start = 1;
//			addInputToBuffer("rx SBC shf ACK");
			break;
		case IDENTIFY_SLOT:
			// next bin = packet.metadata --> TODO impl stepper motor spinning to bin
			if (++current_bin_count < 52)
				send_packet(USART1, build_packet(CAPTURE_IMAGE, current_bin_count));
//		    strcpy(s1, "rx Slot ID = ~~");
//			s1[13] = '0' + (packet.metadata / 10) % 10;
//			s1[14] = '0' + packet.metadata % 10;
//			addInputToBuffer(s1);
			break;
		case REINDEX_SLOT:
//		    strcpy(s2, "rx ReIndex = ~~");
//			s2[13] = '0' + (packet.metadata / 10) % 10;
//			s2[14] = '0' + packet.metadata % 10;
//			addInputToBuffer(s2);
			// TODO impl reindexing/error correction of current_bin_count
			break;
		default:
		    // debug print to LCD screen
		    strcpy(s3, "Unknown packet: 0x~~");
			int h = packet_int >> 4;
			s3[18] = (h >= 10 ? 'a' - 10 : '0') + h;
			h = packet_int & 0xf;
			s3[19] = (h >= 10 ? 'a' - 10 : '0') + h;
			addInputToBuffer(s3);
			break;
	}
}

void redraw() {
	if (needs_reset) {
		draw_screen();
		needs_reset = 0;
	}
	sleep_ms(33); // run @ ~30Hz
}

#define CHECK_SYSTEM_RESET() do { if (got_reset) goto _exec_loop; } while (0)

/**
 * TODOs for integration:
 *  - clean up init's
 *  - add necessary screen state impl's: WakeSyncScreen, Shuffling
 *  - fix and integrate stepper motor control functions to UART handler and shuffling section
 *  - add config strings transmissions to main
 */
int main(void)
{
	// INIT
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


//    init_system();

	int i = 0;
_exec_loop:
	// reset state flags
	got_reset = 0;
	got_mcu_start = 0;
	got_sbc_start = 0;
	current_bin_count = 0;

	// Wait for RESET packet conf
	change_state(WakeSyncScreen); // "Initializing systems" or something on display??
	while (!got_reset) {
		send_packet(USART1, build_packet0(TX_RESET));
		redraw();
	}
	// now, flush all remaining reset packets to avoid reset "boot-loop" (runs ~1s)
	for (i = 0; i < 30; i++) {
		redraw();
	}
	got_reset = 0; // clear reset flag

	// Start configurations
	change_state(CardShuffling); // this is actually the configuration menu.... TODO fix name of screen state

	while(curr_state != Shuffling) { // or however you want to indicate that system is ready to shuffle
		CHECK_SYSTEM_RESET(); // SYSTEM RESET!!!
		if (got_sbc_start) {
			change_state(Shuffling);
			send_packet(USART1, build_packet0(TX_START_SHUFFLE_SBC));
			break;
		}
		redraw();
	}
	if (!got_sbc_start) {
		// TODO add transmission of configurations here
//		for (char *str : list_of_configs) {
//			send_string(USART1, str); // assuming str is properly null-terminated...
//		}
		while (!got_mcu_start) {
			CHECK_SYSTEM_RESET(); // SYSTEM RESET!!!
			send_packet(USART1, build_packet0(TX_START_SHUFFLE_MCU));
			redraw();
		}
	}

	// Start shuffling
	current_bin_count = 0;
	send_packet(USART1, build_packet(CAPTURE_IMAGE, current_bin_count));

	while (current_bin_count < 52) {
		CHECK_SYSTEM_RESET(); // SYSTEM RESET!!!
		redraw();
	}

	// TODO output cards from bins
//	set_stepper_dir(???);
//	gen_N_pulses_wheel(200); // one full rotation to eject all cards... might need to add delays between bins though???


	goto _exec_loop; // inf loop for system loop
}
