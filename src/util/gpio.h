#ifndef __UTIL_GPIO_H__
#define __UTIL_GPIO_H__

#include "stm32f0xx.h"

/**
 * Value Bounds for Variable "Types":
 * - pin_group   {'a' | 'A' | 'b' | 'B' | 'c' | 'C'}
 * - pin         [0, 16)
 * - value       {0 | 1}
 * - altfunc_id  [0, 8)
 */

GPIO_TypeDef *enable_pins(char pin_group);

void enable_input_mode(char pin_group, int pin);
int read_input(char pin_group, int pin); // return value

void enable_output_mode(char pin_group, int pin);
void set_output(char pin_group, int pin, int value);

void set_altfunc(char pin_group, int pin, int altfunc_id);

#endif // __UTIL_GPIO_H__
