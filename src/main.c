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
#include "structs.h"
#include "uart.h"
#include "util/sleep.h"
#include "stepper.h"

int draw_color = 0xffff;
int needs_reset = 1;

char dbg_invalid_packet_str[25] = "Unknown packet: 0x~~";

// Weird stuff

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

//int high = 0;
//
//void TIM7_IRQHandler(void) {
//    TIM7->SR &= ~TIM_SR_UIF;
//    if(high) {
//        GPIOA->BSRR |= GPIO_BSRR_BR_8;
//        high = 0;
//    } else {
//        GPIOA->BSRR |= GPIO_BSRR_BS_8;
//        high = 1;
//    }
//}

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

void TIM15_IRQHandler(void) {
	TIM15->SR &= ~TIM_SR_UIF;
	stepper0_ready_to_pulse = 1;
}

void TIM16_IRQHandler(void) {
	TIM16->SR &= ~TIM_SR_UIF;
	stepper1_ready_to_pulse = 1;
}

// Begin GPIO Exti stuff

void EXTI4_15_IRQHandler(void) {
    gen_N_pulses_dispenser(200);
    gen_N_pulses_wheel(200);
    if(EXTI->PR & EXTI_PR_PR4) { // Up -- 1
        on_press(0);
    	toggle_heartbeat_led();
    	set_gpiob(3, heartbeat);
        EXTI->PR |= EXTI_PR_PR4;
    }
    else if(EXTI->PR & EXTI_PR_PR5) { // Down -- 4
        on_press(1);
    	toggle_heartbeat_led();
		set_gpiob(4, heartbeat);
        EXTI->PR |= EXTI_PR_PR5;
    }
    else if(EXTI->PR & EXTI_PR_PR10) { // Right -- 5
        on_press(2);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR10;
    }
    else if(EXTI->PR & EXTI_PR_PR11) { // Left -- 3
        on_press(3);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR11;
    }
    else if(EXTI->PR & EXTI_PR_PR12) { // Enter -- 2
        on_press(4);
    	toggle_heartbeat_led();
        EXTI->PR |= EXTI_PR_PR12;
    }
}

// End EXTI

char send_str[50];

void sendShuffleData() {
    if(!strcmp(shuffle.game, "BlackJack")) {
        strcpy(send_str,"game:blackjack");
        send_string(USART1, send_str);

        strcpy(send_str,"num_players:~");
        send_str[12] = '0' + shuffle.num_players;
        send_string(USART1, send_str);

        if(shuffle.dealer) {
            strcpy(send_str, "winner:dealer");
            send_string(USART1, send_str);
        } else if(shuffle.table) {
            strcpy(send_str, "winner:table");
            send_string(USART1, send_str);
        } else {
            strcpy(send_str, "winner:~");
            for(int i = 0; i < shuffle.num_players; i++) {
                if(shuffle.winners[i]) {
                    send_str[7] = '0' + i;
                    send_string(USART1, send_str);
                }
            }
        }
    } else if(!strcmp(shuffle.game, "Random")) {
        strcpy(send_str,"game:random");
        send_string(USART1, send_str);
    }
}

#define WHEEL_STEPPER 1
#define CARD_INGEST_STEPPER (1 - WHEEL_STEPPER)

int got_reset = 0;
int got_mcu_start = 0;
int got_sbc_start = 0;
int current_bin_count = 0;
int stored_positions[NUM_BINS] = {0};
int num_bins_dispensed = 0;

int half_step_offset = 0;
int spin_amount = 0;

void USART1_IRQHandler() {
//    USART1->ISR &= ~USART_ISR_RXNE;
	int packet_int = USART1->RDR & 0xff; // reading RDR should clear RXNE flag (line above)
	rx_packet packet = translate_packet(packet_int);
	int h;

	switch (packet.action) {
		case RX_RESET:
			got_reset = 1;
			break;
		case RX_START_SHUFFLE_MCU:
			got_mcu_start = 1;
			break;
		case RX_START_SHUFFLE_SBC:
			got_sbc_start = 1;
			break;
		case IDENTIFY_SLOT:
			// next bin = packet.metadata --> TODO impl stepper motor spinning to bin
			wait_for_turnable(WHEEL_STEPPER);
			turn_motor(WHEEL_STEPPER, numStepsBetweenBins(packet.metadata, 0));
			wait_for_turnable(WHEEL_STEPPER);
			turn_motor(CARD_INGEST_STEPPER, 200);
			sleep_ms(1000); // wait for ingest to complete

			// online card output
			stored_positions[packet.metadata] = 1;
			if (stored_positions[num_bins_dispensed]) {
				// turn to bin #num_bins_dispensed
				spin_amount = numStepsBetweenBins(num_bins_dispensed, 0);
				// turn another 39 + 0.5 bins
				spin_amount += numStepsToSkipBins(39, 0);
				half_step_offset = numStepsToSkipBins(1, 0) / 2;
				spin_amount += half_step_offset;
				turn_motor(WHEEL_STEPPER, spin_amount);
				numStepsToSkipBins(1, 1); // update current_bin
				// switch direction of stepper
				reverse_motor_dir(WHEEL_STEPPER);
				// dispense cards
				while (stored_positions[num_bins_dispensed]) {
					// turn 0.5 to reset
					spin_amount = half_step_offset;
					// compute next offset
					half_step_offset = numStepsToSkipBins(1, 1);
					// turn 0.5 to re-offset/drop
					spin_amount += half_step_offset / 2;
					half_step_offset -= half_step_offset / 2;
					// update index
					wait_for_turnable(WHEEL_STEPPER);
					turn_motor(WHEEL_STEPPER, spin_amount);
					num_bins_dispensed++;
				}
				// switch direction of stepper
				reverse_motor_dir(WHEEL_STEPPER);
				// turn 0.5 bins
				turn_motor(WHEEL_STEPPER, numStepsToSkipBins(1, 0) - half_step_offset);
			}

			// process next card
			if (++current_bin_count < 52)
				send_packet(USART1, build_packet(CAPTURE_IMAGE, current_bin_count));
			break;
		case REINDEX_SLOT:
			// TODO impl reindexing/error correction of current_bin_count
			break;
		default:
		    // debug print to LCD screen
			h = packet_int >> 4;
			dbg_invalid_packet_str[18] = (h >= 10 ? 'a' - 10 : '0') + h;
			h = packet_int & 0xf;
			dbg_invalid_packet_str[19] = (h >= 10 ? 'a' - 10 : '0') + h;
			addInputToBuffer(dbg_invalid_packet_str);
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
 *  - implement and integrate dc motor control around shuffle block
 */
int main(void)
{
	// INIT
	init_buttons();
	LCD_Setup();

	init_opm_tim();
//    init_system();

	set_motor_dir(0, 0); // stepper 0 = near power jack
	set_motor_dir(1, 0); // stepper 1 = near BJTs

	sleep_ms(10); // wait for all changes to propagate if necessary??

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
	    sendShuffleData();
		while (!got_mcu_start) {
			CHECK_SYSTEM_RESET(); // SYSTEM RESET!!!
			send_packet(USART1, build_packet0(TX_START_SHUFFLE_MCU));
			redraw();
		}
	}

	// Start shuffling
	// TODO enable/disable dc motors around shuffling block
//	enable_dc_motors();

	current_bin_count = 0;
	for (i = 0; i < NUM_BINS; i++)
		stored_positions[i] = 0;
	num_bins_dispensed = 0;

	send_packet(USART1, build_packet(CAPTURE_IMAGE, current_bin_count));

	while (current_bin_count < 52) {
		CHECK_SYSTEM_RESET(); // SYSTEM RESET!!!
		redraw();
	}

//	disable_dc_motors();

	// output cards from bins

	// turn to bin #num_bins_dispensed
	spin_amount = numStepsBetweenBins(num_bins_dispensed, 0);
	// turn another 39 + 0.5 bins
	spin_amount += numStepsToSkipBins(39, 0);
	half_step_offset = numStepsToSkipBins(1, 0) / 2;
	spin_amount += half_step_offset;
	turn_motor(WHEEL_STEPPER, spin_amount);
	numStepsToSkipBins(1, 1); // update current_bin
	// switch direction of stepper
	reverse_motor_dir(WHEEL_STEPPER);
	// dispense cards
	while (stored_positions[num_bins_dispensed]) {
		// turn 0.5 to reset
		spin_amount = half_step_offset;
		// compute next offset
		half_step_offset = numStepsToSkipBins(1, 1);
		// turn 0.5 to re-offset/drop
		spin_amount += half_step_offset / 2;
		half_step_offset -= half_step_offset / 2;
		// update index
		wait_for_turnable(WHEEL_STEPPER);
		turn_motor(WHEEL_STEPPER, spin_amount);
		num_bins_dispensed++;
	}

	goto _exec_loop; // inf loop for system loop
}
