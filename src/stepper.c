/*
 * StepperMotor.c
 *
 *  Created on: Apr 14, 2023
 *      Author: ztwil
 */

#include "stepper.h"

#include "util/gpio.h"
#include "util/sleep.h"

#include "stm32f0xx.h"

int getNumSteps(int bin) {
    static int bin_to_steps[NUM_BINS] = {0, 4, 8, 11, 15, 19, 23, 27, 31, 35, 38, 42, 46, 50, 54, 58, 61, 65, 69, 73, 77, 81, 85, 88, 92, 96, 100, 104, 108, 111, 115, 119, 123, 127, 131, 135, 138, 142, 146, 150, 154, 158, 161, 165, 169, 173, 177, 181, 185, 188, 192, 196};
    return bin_to_steps[(bin % NUM_BINS + NUM_BINS) % NUM_BINS];
}

int numStepsBetweenBins(int to_bin, int dir) { // DIR = 1 is outputting, DIR = 0 is inputting
    int diff = getNumSteps(to_bin) - getNumSteps(current_bin);
    current_bin = to_bin; // update current_bin
    return (STEPS_PER_ROT + (dir ? diff : -diff)) % STEPS_PER_ROT;
}

int numStepsToSkipBins(int n_bins, int dir) { // DIR = 1 is outputting, DIR = 0 is inputting
	return numStepsBetweenBins(current_bin + (dir ? n_bins : -n_bins), dir);
}

int is_valid_stepper(int stepper_num) {
	return stepper_num == 0 || stepper_num == 1;
}

int *get_ready_flag(int stepper_num) {
	if (stepper_num == 0)
		return &stepper0_ready_to_pulse;
	if (stepper_num == 1)
		return &stepper1_ready_to_pulse;
	return 0;
}

TIM_TypeDef *get_tim(int stepper_num) {
	if (stepper_num == 0)
		return TIM15;
	if (stepper_num == 1)
		return TIM16;
	return 0;
}

gpio_pin get_enable_pin(int stepper_num) {
	if (stepper_num == 0)
		return STEPPER0_ENABLE_PIN;
	if (stepper_num == 1)
		return STEPPER1_ENABLE_PIN;
	return NULL_PIN;
}

gpio_pin get_dir_pin(int stepper_num) {
	if (stepper_num == 0)
		return STEPPER0_DIR_PIN;
	if (stepper_num == 1)
		return STEPPER1_DIR_PIN;
	return NULL_PIN;
}

gpio_pin get_pulse_pin(int stepper_num) {
	if (stepper_num == 0)
		return STEPPER0_PULSE_PIN;
	if (stepper_num == 1)
		return STEPPER1_PULSE_PIN;
	return NULL_PIN;
}

void wait_for_flag(int *flag) {
	while (!*flag)
		sleep_micros(1); // TODO pick good sleep duration
}

void set_motor_dir(int stepper_num, int dir) {
	if (!is_valid_stepper(stepper_num)) return;

	int *ready_flag = get_ready_flag(stepper_num);
	if (!ready_flag) return;

	wait_for_flag(ready_flag);
	*ready_flag = 0;

	gpio_pin dir_pin = get_dir_pin(stepper_num);
	set_output(dir_pin.pin_group, dir_pin.pin, dir);

	sleep_micros(500); // TODO figure out minimum config propagation delay here

	*ready_flag = 1;
}

void reverse_motor_dir(int stepper_num) {
	gpio_pin dir_pin = get_dir_pin(stepper_num);
	int dir = read_output(dir_pin.pin_group, dir_pin.pin);
	set_motor_dir(stepper_num, !dir);
}

void turn_motor(int stepper_num, int n_steps) {
	if (!is_valid_stepper(stepper_num)) return;

	int *ready_flag = get_ready_flag(stepper_num);
	if (ready_flag) *ready_flag = 0;

	TIM_TypeDef *opm_tim = get_tim(stepper_num);
	if (!opm_tim) return;
	opm_tim->RCR = n_steps - 1;
	opm_tim->CR1 |= TIM_CR1_CEN;

	// TODO maybe break steps into "bite-sized" chunks to improve consistency??
}

void wait_for_turnable(int stepper_num) {
	if (!is_valid_stepper(stepper_num)) return;

	int *ready_flag = get_ready_flag(stepper_num);
	if (ready_flag) wait_for_flag(ready_flag);
}

void init_stepper_timer_interrupts() {
	TIM15->DIER |= TIM_DIER_UIE;
	TIM16->DIER |= TIM_DIER_UIE;
}

void set_motor_enable(int stepper_num, int en) {
    if(!is_valid_stepper(stepper_num)) return;

    *get_ready_flag(stepper_num) = !en;
    gpio_pin en_pin = get_enable_pin(stepper_num);
    set_output(en_pin.pin_group,en_pin.pin,en);
}
