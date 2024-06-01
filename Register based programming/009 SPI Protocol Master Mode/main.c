#include "stm32f4xx.h"                  // Device header
#include "System_Clock.h"
#include <stdint.h>

void GPIO_config (void);
void SPI1_config (void);
void SPI1_control_NSS (uint8_t state);
void SPI1_transmit (uint8_t *data, uint32_t size);
void SPI1_receive (uint8_t *data, uint32_t size);

static uint8_t Tx_data[5] = {0x12, 0x42, 0xA2, 0xBA, 0x6F};
static uint8_t Rx_data[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

int main (void){
	SystemClock_config();
	GPIO_config();
	SPI1_config();
	
	// Send 5 bytes of data
	SPI1_transmit(Tx_data, 5);	
	// Read 5 bytes of data
	SPI1_receive(Rx_data, 5);
	return 0;
}

void GPIO_config (void){

/*
	PA4 - SPI1_NSS
	PA5 - SPI1_SCLK
	PA6 - SPI1_MISO
	PA7 - SPI1_MOSI
*/

	// Enable GPIOA clock
	RCC->AHB1ENR |= (1UL<<0);
	// Set PA5, PA6, PA7 as SCLK, MISO, MOSI
	GPIOA->MODER |= (2UL<<10) | (2UL<<12) | (2UL<<14); // Alternative function mode
	GPIOA->OSPEEDR |= (3UL<<10) | (3UL<<12) | (3UL<<14); // Very high output speed
	GPIOA->AFR[0] |= (5UL<<20) | (5UL<<24) | (5UL<<28); // Alternative function 5 -> SPI1
	// Set PA4 as general output CSS
	GPIOA->MODER |= (1UL<<8); // Output mode
	GPIOA->OTYPER &= ~(1UL<<4); // Output push-pull
	GPIOA->OSPEEDR |= (3UL<<8); // Very high output speed 
	GPIOA->ODR |= (1UL<<4); // Set as high
}

void SPI1_config (void){
	
/*
	APB2CLK = 84MHz
	SPI1_div = 16 (bit3-5 of SPI1_CR1)
	SPI1_baudrate = APB2CLK / SPI1_div = 5.25MHz
*/
	
	// Enable SPI1 clock
	RCC->APB2ENR |= (1UL<<12);
	// CPOL=0, CPHA=0
  SPI1->CR1 &= ~((1UL<<0) | (1UL<<1));   
	// Master Mode
  SPI1->CR1 |= (1UL<<2);  
	// Setup Baudrate 
  SPI1->CR1 |= (3UL<<3);
	// Send MSB first
  SPI1->CR1 &= ~(1UL<<7);
	// Software Slave Management, use GPIO instead of SPI1_NSS
  SPI1->CR1 |= (1UL<<8) | (1UL<<9);
	// Full-duplex
  SPI1->CR1 &= ~(1UL<<10);
	// Dataframe has 8 bits data
  SPI1->CR1 &= ~(1UL<<11); 
  SPI1->CR2 = 0;
	// Enable SPI1
	SPI1->CR1 |= (1UL<<6);   
}

void SPI1_control_NSS (uint8_t state){
	if(state) GPIOA->ODR |= (1UL<<4);
	else GPIOA->ODR &= ~(1UL<<4);
}

void SPI1_transmit (uint8_t *data, uint32_t size){
	uint32_t i = 0;
	// Reset NSS to start trasnmitting
	SPI1_control_NSS(0);
	while (i<size){
		// Wait for buffer tobe empty (TXE flag)
	  while (!((SPI1->SR)&(1<<1)));  
	  // Load the data into the Data Register
		SPI1->DR = data[i];
	  i++;
	}	
	// Wait for buffer is empty (TXE flag)
	while (!((SPI1->SR)&(1<<1)));
	// Wait for SPI to ready (BSY flag)	
	while (((SPI1->SR)&(1<<7)));  
	// Clear the Overrun flag by reading DR and SR
	i = SPI1->DR;
	i = SPI1->SR;
	// Set NSS to end transmitting
	SPI1_control_NSS(1);
}

void SPI1_receive (uint8_t *data, uint32_t size){
	// Reset NSS to start receiving
	SPI1_control_NSS(0);
	while (size){
		// Wait SPI to ready to communication (BSY flag)
		while ((SPI1->SR)&(1UL<<7));
		// Send dummy data to receive data
		SPI1->DR = 0;
		// Wait for Rx buffer to get some data
		while (!((SPI1->SR) &(1UL<<0)));
		*data++ = (uint8_t) (SPI1->DR);
		size--;
	}
	// Set NSS to end receiving
	SPI1_control_NSS(1);
}
