/*
 * gpio.c
 *
 *  Created on: Apr 13, 2023
 *      Author: up
 */

#include "stm32f0xx.h"
#include "gpio.h"

GPIO_TypeDef *enable_pins(char pin_group) {
	switch (pin_group) {
		case 'a':
		case 'A':
			RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
			return GPIOA;

		case 'b':
		case 'B':
			RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
			return GPIOB;

		case 'c':
		case 'C':
			RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
			return GPIOC;

		default:
			return 0x0;
	}
}

void enable_input_mode(char pin_group, int pin) {
	read_input(pin_group, pin);
}
int read_input(char pin_group, int pin) {
	// enable pin group, get group struct
	GPIO_TypeDef *gpio = enable_pins(pin_group);

	// set MODER
	int shift_moder = 2 * pin;

	gpio->MODER &= ~(0b11 << shift_moder);

	// get value
	return (gpio->IDR >> pin) & 0b1;
}

void enable_output_mode(char pin_group, int pin) {
	set_output(pin_group, pin, 0);
}
void set_output(char pin_group, int pin, int value) {
	// enable pin group, get group struct
	GPIO_TypeDef *gpio = enable_pins(pin_group);

	// set MODER
	int shift_moder = 2 * pin;

	gpio->MODER &= ~(0b11 << shift_moder);
	gpio->MODER |=  (0b01 << shift_moder);

	// set value
	gpio->BSRR = 1 << (value ? pin : (16 + pin));
}

void set_altfunc(char pin_group, int pin, int altfunc_id) {
	// enable pin group, get group struct
	GPIO_TypeDef *gpio = enable_pins(pin_group);

	// set MODER
	int shift_moder = 2 * pin;

	gpio->MODER &= ~(0b11 << shift_moder);
	gpio->MODER |=  (0b10 << shift_moder);

	// set AFR
	int shift_afr = 4 * (pin % 8);
	int afr_ind = pin / 8;

	gpio->AFR[afr_ind] &= ~(0b1111 << shift_afr);
	gpio->AFR[afr_ind] |=  (altfunc_id << shift_afr);
}
