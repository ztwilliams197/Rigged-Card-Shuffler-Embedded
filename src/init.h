/*
 * init.h
 *
 *  Created on: Dec 4, 2021
 *      Author: ztwil
 */

#ifndef INIT_H_
#define INIT_H_

#include "stm32f0xx.h"

void init_spi2_tft(void);
void init_lcd_spi(void);
void init_refresh_tim();

void init_tim1();
void init_opm_tim();

void init_uart();

void init_buttons();

void init_DC_motors(void);


#endif /* INIT_H_ */
