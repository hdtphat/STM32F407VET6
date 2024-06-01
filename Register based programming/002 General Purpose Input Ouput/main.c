#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h>

void GPIO_config (void);
void Wasting_time (uint32_t n);
static uint32_t time = 100; // change this in debug mode

int main (void){
	SystemClock_config();
	GPIO_config();
	while(1){
		GPIOA->ODR ^= (1UL<<7); // toggle PA7
		Wasting_time(time);
	}
}

void GPIO_config (void){
	// enable GPIOA clock 
	RCC->AHB1ENR |= (1UL<<0);
	// set PA7 as general output
	GPIOA->MODER |= (1UL<<14);
	// set output type as push-pull
	GPIOA->OTYPER &= ~(1UL<<7) ;
	// set medium speed
	GPIOA->OSPEEDR |= (1UL<<14);
	// no pull-up nor pull-down
	GPIOA->PUPDR |= (1UL<<14);
}

void Wasting_time (uint32_t n){
	for(uint32_t i=0; i<n; i++){
		uint32_t count = 0xFFFF;
		while(count--);
	}
}
