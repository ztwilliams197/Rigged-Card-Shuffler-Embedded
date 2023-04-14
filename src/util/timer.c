#include "timer.h"

#include "stm32f0xx.h"

#define STEPPER_BIN_HZ (600)
#define STEPPER_PULSE_WIDTH (40)

void init_opm(TIM_TypeDef *tim) {
	// TODO reset all other important fields to 0's just in case
	tim->PSC = (48000000 / STEPPER_BIN_HZ / STEPPER_PULSE_WIDTH / 2) - 1;
	tim->ARR = 2 * STEPPER_PULSE_WIDTH - 1;
	tim->CCR1 = STEPPER_PULSE_WIDTH - 1;
	tim->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE;
	tim->CCER |= TIM_CCER_CC1E;
	tim->BDTR |= TIM_BDTR_MOE;
    tim->CR1 |= TIM_CR1_OPM | TIM_CR1_ARPE;
}
