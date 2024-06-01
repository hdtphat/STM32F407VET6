#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h>

void GPIO_config (void);
void TIM2_PWM_config (void);

int main (void){
	SystemClock_config();
	GPIO_config();
	TIM2_PWM_config();
	while(1);
}

void GPIO_config (void){
	// Enable SYSCFG clock
	RCC->APB2ENR |= (1UL<<14);
	// Enable GPIOA clock
	RCC->AHB1ENR |= (1UL<<0);
	// Set PA1 and PA2 as alternated function
	GPIOA->MODER |= (2UL<<2) | (2UL<<4);
	// Very high output speed
	GPIOA->OSPEEDR |= (3UL<<2) | (3UL<<4);
	// Set PA1 and PA2 as TIM2_CH2 and TIM2_CH3
	GPIOA->AFR[0] |= (1UL<<4) | (1UL<<8);
}

void TIM2_PWM_config (void){
	
/*
	APB1CLK = 84MHz
	TIM2CLK = APB1CLK / TIM2PSC
	PWM_frequency = TIM2CLK / TIM2ARR
*/

	// Enable TIM2 clock 
	RCC->APB1ENR |= (1UL<<0);
	// Set TIM2 prescaler
	TIM2->PSC = 840-1; 
	// Set max count and reset counter
	TIM2->ARR = 2000;
	TIM2->CNT = 0;
	// set chanel CCR
	TIM2->CCR2 = 1000; // CH2 PA1 50%
	TIM2->CCR3 = 500; // CH3 PA2 25%
	// set to PWM1 mode 1
	TIM2->CCMR1 |= (6UL<<12);
	TIM2->CCMR2 |= (6UL<<4);
	// enable chanel CH2 and CH3
	TIM2->CCER |= (1UL<<4) | (1UL<<8);
	// enable counter to start counting
	TIM2->CR1 |= (1UL<<0);
}
