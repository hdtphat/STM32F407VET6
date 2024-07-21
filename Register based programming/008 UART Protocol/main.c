#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h> 

void GPIO_config (void);
void UART1_config (void);
void UART1_send_string (char *str);
void USART1_IRQHandler (void);

static char Rx_Buffer[100];
static uint8_t Rx_count = 0;

int main (void)
{
	SystemClock_config();
	GPIO_config();
	UART1_config();
	
	UART1_send_string("Hello world!\n");
	while(1);
}

void GPIO_config (void)
{
	// Enable GPIOA clock 
	RCC->AHB1ENR |= (1UL<<0);
	// Set PA9 and PA10 as alternative function
	GPIOA->MODER |= (2UL<<18) | (2UL<<20);
	// Very high output speed
	GPIOA->OSPEEDR |= (2UL<<18) | (2UL<<20);
	// Set PA9 and PA10 as UART1_Tx and UART1_Rx
	GPIOA->AFR[1] |= (7UL<<4) | (7UL<<8);
}

void UART1_config (void)
{
	
/*
	How to define Baudrate
	
	APB2CLK = 84MHz
	Target_Baudrate = 9600Hz
	USART1_prescaler = (APB2CLK * 1000000) / (16 * Target_Baudrate) = 546.875
	
	Set bit4-15 of USART1_BRR = 546
	Set bit0-3 of USART1_BRR = (0.875 * 16) = 14
*/
	
	// Enable USART1 clock
	RCC->APB2ENR |= (1UL<<4);
	// Set message frame
	USART1->CR1 &= ~(1UL<<12); // 1 start bit - 8 data bits - n stop bit
	USART1->CR2 &= ~(3UL<<12); // Set number of Stop bit to 1
	USART1->CR1 &= ~(1UL<<10); // Disable pairity bit
	// Set baudrate
	USART1->BRR = (14UL<<0) | (546UL<<4);
	// Enable Receive/Transmit mode
	USART1->CR1 |= (1UL<<2) | (1UL<<3);
	// Enable Rx interupt
	USART1->CR1 |= (1UL<<5);
	// Enable global interupt
	__enable_irq();
	NVIC_EnableIRQ(USART1_IRQn);
	// Enable USART1
	USART1->CR1 |= (1UL<<13);  
}

void USART1_IRQHandler (void) // Receive a string that end with ';'
{
	// Clear Rx interupt flag
	USART1->SR &= ~(1UL<<5);
	// Get a string of data
	if (USART1->DR != ';'){
		Rx_Buffer[Rx_count] = (char)(USART1->DR);
		Rx_count++;
	} else {
		Rx_Buffer[Rx_count] = '\0';
		Rx_count = 0;
		
	}
}

void UART1_send_string (char *str)
{
	while(*str){
		USART1->DR = *str; 
		while (!(USART1->SR & (1UL<<6)));
		str++;
	}
}
