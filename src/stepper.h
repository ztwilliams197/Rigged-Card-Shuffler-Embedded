/*
 * StepperMotor.h
 *
 *  Created on: Apr 14, 2023
 *      Author: ztwil
 */

#ifndef __STEPPER_H_
#define __STEPPER_H_

#include "util/gpio.h"

#define NUM_BINS 52
#define STEPS_PER_ROT 200

int current_bin;

int getNumSteps(int bin);
int numStepsBetweenBins(int to_bin, int dir);
int numStepsToSkipBins(int n_bins, int dir);

// stepper_num = 0 (close to power jack) or 1 (close to BJTs)

int stepper0_ready_to_pulse;
int stepper1_ready_to_pulse;

void set_motor_dir(int stepper_num, int dir);
void reverse_motor_dir(int stepper_num);
void turn_motor(int stepper_num, int n_steps);
void wait_for_turnable(int stepper_num);

#define STEPPER0_PULSE_PIN   ((gpio_pin) {.pin_group='A', .pin=2})
#define STEPPER0_DIR_PIN     ((gpio_pin) {.pin_group='A', .pin=4})
#define STEPPER0_ENABLE_PIN  ((gpio_pin) {.pin_group='A', .pin=5})

#define STEPPER1_PULSE_PIN   ((gpio_pin) {.pin_group='A', .pin=6})
#define STEPPER1_DIR_PIN     ((gpio_pin) {.pin_group='A', .pin=11})
#define STEPPER1_ENABLE_PIN  ((gpio_pin) {.pin_group='A', .pin=12})

void init_stepper_timer_interrupts();


#endif // __STEPPER_H_
