#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h>

void GPIO_config (void);
void TIM4_config (void);
void TIM4_IRQHandler(void);

int main (void){
	SystemClock_config();
	GPIO_config();
	TIM4_config();
	while(1);
}

void GPIO_config (void){
	// Enable GPIOA clock
	RCC->AHB1ENR |= (1UL<<0);
	// Set PA7 as output
	GPIOA->MODER |= (1UL<<14); // output mode
	GPIOA->OTYPER &= ~(1UL<<0); // push-pull
	GPIOA->OSPEEDR |= (1UL<<14); // medium speed
	GPIOA->PUPDR |= (1UL<<14); // pull-up
	// Reset all pin
	GPIOA->ODR = 0;
}

void TIM4_config (void){

/*
	APB1CLK = 42MHz = 42000000Hz
	APB1_Timers_CLK = APB1CLK * 2 = 84000000Hz (APB1 prescaler != 1)
	TIM4CLK = APB1_Timers_CLK / TIM4PSC = 100000Hz
	time = TIM4ARR / TIM4CLK = 0.5s
*/	

	// Enable TIM4 clock 
	RCC->APB1ENR |= (1UL<<2);
	// Set TIM4 prescaler 
	TIM4->PSC = 840-1; // TIM4CLK = 0.1MHz, each count takes 10ns
	// Set max count 
	TIM4->ARR = 50000; // timer interupt every 500ms
	// Reset counter
	TIM4->CNT = 0;
	// Enable timer interrupt
	TIM4->DIER |= (1UL<<0);
	NVIC_EnableIRQ(TIM4_IRQn);	
	// Enable counter 
	TIM4->CR1 |= (1UL<<0);
}

void TIM4_IRQHandler(void){
	// Clear interupt flag
	TIM4->SR = 0;
	// Do something 
	GPIOA->ODR ^= (1UL<<7);
}
