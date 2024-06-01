#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h>

void GPIO_config (void);
void External_Interupt_config (void);
void EXTI0_IRQHandler (void);
static int count = 0;

int main (void){
	SystemClock_config();
	GPIO_config();
	External_Interupt_config();
	while(1);
}

void GPIO_config (void){
	// Enable GPIOA clock
	RCC->AHB1ENR |= (1UL<<0);
	// Setup PA0 as input
	GPIOA->MODER &= ~(3UL<<0); // input mode
	GPIOA->OSPEEDR |= (1UL<<0); // medium speed
	GPIOA->PUPDR |= (1UL<<0); // pull-up
	// Setup PA7 as output
	GPIOA->MODER |= (1UL<<14); // output mode
	GPIOA->OTYPER &= ~(1UL<<7); // output push-pull
	GPIOA->OSPEEDR |= (1UL<<14); // medium speed
	GPIOA->PUPDR |= (1UL<<0); // pull-up
	
}

void External_Interupt_config (void){
	// Enable SYSCFGR
	RCC->APB2ENR |= (1UL<<14);
	// Configure the EXTI Registers (port: 0-A, 1-B, 2-C, 3-D, 4-E)
	SYSCFG->EXTICR[0] = 0x0000; // EXTICR1: pin3 <- pin0
	SYSCFG->EXTICR[0] = 0x0000; // EXTICR1: pin7 <- pin4
	SYSCFG->EXTICR[0] = 0x0000; // EXTICR1: pin11 <- pin8
	SYSCFG->EXTICR[0] = 0x0000; // EXTICR1: pin15 <- pin12
	// Disable the EXTI Mask
	EXTI->IMR |= (1UL<<0);
	// Configure the Rising Edge Trigger
	EXTI->RTSR |= (1UL<<0);
	// Configure the Falling Edge Trigger
	EXTI->FTSR &= ~(1UL<<0);
	// Set interupt priority
	NVIC_SetPriority (EXTI0_IRQn, 1);
	// Enable interupt
	NVIC_EnableIRQ (EXTI0_IRQn);
}

void EXTI0_IRQHandler (void){

	if (EXTI->PR & (1UL<<0)){
		// clear interupt flag
		EXTI->PR |= (1UL<<0);  
		// do something
		count++;
		if(!(count%10)){
			GPIOA->ODR ^= (1UL<<7);
			count = 0;
		}
	}
}
