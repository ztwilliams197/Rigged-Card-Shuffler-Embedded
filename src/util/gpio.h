#ifndef __SRC_GPIO_H__
#define __SRC_GPIO_H__

#include "stm32f0xx.h"

/**
 * @param pin_group  {'a' | 'A' | 'b' | 'B' | 'c' | 'C'}
 */
GPIO_TypeDef *enable_pins(char pin_group);

/**
 * @param pin_group  {'a' | 'A' | 'b' | 'B' | 'c' | 'C'}
 * @param pin        [0, 16)
 *
 * @return {0 | 1}
 */
void enable_input_mode(char pin_group, int pin);
int read_input(char pin_group, int pin);
/**
 * @param pin_group  {'a' | 'A' | 'b' | 'B' | 'c' | 'C'}
 * @param pin        [0, 16)
 * @param value      {0 | 1}
 */
void enable_output_mode(char pin_group, int pin);
void set_output(char pin_group, int pin, int value);
/**
 * @param pin_group   {'a' | 'A' | 'b' | 'B' | 'c' | 'C'}
 * @param pin         [0, 16)
 * @param altfunc_id  [0, 8)
 */
void set_altfunc(char pin_group, int pin, int altfunc_id);

#endif // __SRC_GPIO_H__
