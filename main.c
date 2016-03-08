/*
 * main.c
 *
 *  Created on: Feb 27, 2016
 *      Author: danielwalsh
 */

#include "TM4C123GH6PM.h"
#include "bsp.h"

/*.........................................................................*/
volatile uint32_t adcResult = 0;

/* adc1 interrupt handler..................................................*/
void ADC1Seq3_IRQHandler(void) {
	adcResult = ADC1->SSFIFO3;
	ADC1->ISC = (1<<3);
}


/*.........................................................................*/
void SysTick_Init(void) {
	SYSCTL->RCGCADC  =  (1<<1);
	SYSCTL->RCGCGPIO =  (1<<4)|(1<<5);
}

/*.........................................................................*/
void GPIOF_Init(void) {
	GPIOF->DEN       =  0xFF;
	GPIOF->AFSEL     =  0x00;
	GPIOF->DIR       =  0xFF;
	GPIOF->DATA      =  (1<<1);
}

/*.........................................................................*/
void GPIOE_Init(void) {
	GPIOE->DIR      &= ~(1<<1);
	GPIOE->AFSEL     =  (1<<1);
	GPIOE->DEN      &= ~(1<<1);
	GPIOE->AMSEL     =  (1<<1);
}

/* adc1 initialization......................................................*/
void ADC1_Init(void) {
	ADC1->ACTSS     &= ~(1<<3);
	ADC1->EMUX       =  (0x0F<<12);
	ADC1->SSMUX3     =  (1<<1);
	ADC1->SSCTL3     =  0x06;
	ADC1->IM         =  (1<<3);
	ADC1->ACTSS     |=  (1<<3);
	ADC1->ISC        =  (1<<3);
}

/* entry point..............................................................*/
int main() {
	SysTick_Init();
	GPIOF_Init();
	GPIOE_Init();
	ADC1_Init();
	NVIC_EnableIRQ(ADC1SS3_IRQn);
	while (1) {
		if (adcResult > 2048) {
			GPIOF->DATA |=  (1<<1);
		} else {
			GPIOF->DATA &= ~(1<<1);
		}
	}

	return 0;
}
