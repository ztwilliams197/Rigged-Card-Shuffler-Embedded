/*
 * init.c
 *
 *  Created on: Dec 4, 2021
 *      Author: ztwil
 */

#include "stm32f0xx.h"
#include "init.h"

void init_spi2_tft(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER15); // PB13 = SPI2_SCK  PB15 = SPI2_MOSI
    GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;

    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    SPI2->CR1 &= ~SPI_CR1_SPE;
    SPI2->CR1 &= ~(SPI_CR1_BR);
    SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM;
    SPI2->CR1 |= SPI_CR1_SSI;
    SPI2->CR2 &= ~SPI_CR2_DS_3;
    SPI2->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_FRXTH;
    SPI2->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER11 | GPIO_MODER_MODER14); // PB8 = CS  PB11 = RST  PB14 = DC
    GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0;
    init_spi2_tft();
}

void init_tim1(void) {
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~0x00ff0000;
    GPIOA->MODER |=  0x00aa0000;
    GPIOA->AFR[1] &= ~0xffff;
    GPIOA->AFR[1] |=  0x2222;

    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->PSC = 1000-1;
    TIM1->ARR = 60-1;

    TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC2M_0);
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
    TIM1->CCMR2 &= ~(TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC4M_0);
    TIM1->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);

    TIM1->CCMR2 |= TIM_CCMR2_OC4PE;

    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    TIM1->CCR1 = (1 - (50 / 100.0)) * (TIM1->ARR + 1);

    TIM1->CR1 |= TIM_CR1_CEN;
}

void init_opm_tim15(void) {
    // Init GPIO Output for alternate function
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~0x30; // A2 -- stepper 1
    GPIOA->MODER |=  0x20;
    GPIOA->AFR[0] &= ~0xf00;

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 1000-1;
    TIM15->ARR = 80-1;
    TIM15->CCR1 = 40-1;
    TIM15->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE;
    TIM15->CCER |= TIM_CCER_CC1E;
    TIM15->BDTR |= TIM_BDTR_MOE;
    TIM15->CR1 |= TIM_CR1_OPM | TIM_CR1_ARPE;
}

void init_opm_tim16(void) {
    // Init GPIO Output for alternate function
    RCC->AHBENR  |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~0x3000; // A6 -- stepper 2
    GPIOA->MODER |=  0x2000;
    GPIOA->AFR[0] &= ~0xf000000;
    GPIOA->AFR[0] |= 0x5000000;

    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;
    TIM16->PSC = 1000-1;
    TIM16->ARR = 80-1;
    TIM16->CCR1 = 40-1;
    TIM16->CCMR1 |= TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE;
    TIM16->CCER |= TIM_CCER_CC1E;
    TIM16->BDTR |= TIM_BDTR_MOE;
    TIM16->CR1 |= TIM_CR1_OPM | TIM_CR1_ARPE;
}

void init_opm_tim(void) {
	init_opm_tim15();
	init_opm_tim16();
}

void init_buttons() {
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    GPIOC->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5 | GPIO_PUPDR_PUPDR10 | GPIO_PUPDR_PUPDR11 | GPIO_PUPDR_PUPDR12);
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR10_0 | GPIO_PUPDR_PUPDR11_0 | GPIO_PUPDR_PUPDR12_0;

    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI4_PC;
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PC;
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PC;
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI11_PC;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI12_PC;

    EXTI->RTSR &= ~(EXTI_RTSR_TR4 | EXTI_RTSR_TR5 | EXTI_RTSR_TR10 | EXTI_RTSR_TR11 | EXTI_RTSR_TR12);
    EXTI->FTSR |= EXTI_FTSR_TR4 | EXTI_FTSR_TR5 | EXTI_FTSR_TR10 | EXTI_FTSR_TR11 | EXTI_FTSR_TR12;
    EXTI->IMR |= EXTI_IMR_MR4 | EXTI_IMR_MR5 | EXTI_IMR_MR10 | EXTI_IMR_MR11 | EXTI_IMR_MR12;

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn,0);
}
