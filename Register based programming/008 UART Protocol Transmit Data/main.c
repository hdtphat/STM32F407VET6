#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h> 

void GPIO_config (void);
void UART1_config (void);
void UART1_send_char (char c);
void UART1_send_string (char *str);
char UART1_get_char (void);

int main (void){
	SystemClock_config();
	GPIO_config();
	UART1_config();
	
	while(1){
		UART1_send_string("Hello world!\n");
	}
}

void GPIO_config (void){
	// Enable GPIOA clock 
	RCC->AHB1ENR |= (1UL<<0);
	// Set PA9 and PA10 as alternative function
	GPIOA->MODER |= (2UL<<18) | (2UL<<20);
	// Very high output speed
	GPIOA->OSPEEDR |= (2UL<<18) | (2UL<<20);
	// Set PA9 and PA10 as UART1_Tx and UART1_Rx
	GPIOA->AFR[1] |= (7UL<<4) | (7UL<<8);
}

void UART1_config (void){
	
/*
	How to define Baudrate
	
	APB2CLK = 84MHz = 84000000Hz
	Target_Baudrate = 9600Hz
	USART1_prescaler = (APB2CLK * 1000000) / (16 * Target_Baudrate) = 546.875
	
	Set bit4-15 of USART1_BRR = 546
	Set bit0-3 of USART1_BRR = (0.875 * 16) = 14
*/
	
	// Enable USART1 clock
	RCC->APB2ENR |= (1UL<<4);
	// Enable UART1
	USART1->CR1 |= (1UL<<13);  
	// Set message frame
	USART1->CR1 &= ~(1UL<<12); // 1 start bit - 8 data bits - n stop bit
	USART1->CR2 &= ~(3UL<<12); // Set number of Stop bit to 1
	USART1->CR1 &= ~(1UL<<10); // Disable pairity bit
	// Set baudrate
	USART1->BRR = (14UL<<0) | (546UL<<4);
	// enable Receive/Transmit mode
	USART1->CR1 |= (1UL<<2) | (1UL<<3); 
}

void UART1_send_char (char c){
	// Load and wait for data to be transmited
	USART1->DR = c; 
	while (!(USART1->SR & (1UL<<6)));
}

void UART1_send_string (char *str){
	while(*str){
		UART1_send_char(*str);
		str++;
	}
}

char UART1_get_char (void){
	char Temp;
	// Wait for RXNE flag
	while (!(USART1->SR & (1UL<<5)));  
	// Read the data. 
	Temp = (USART1->DR & 0xFF);
	return Temp;
}
