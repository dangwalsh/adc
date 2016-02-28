/*
 * main.c
 *
 *  Created on: Feb 27, 2016
 *      Author: danielwalsh
 */

#include "TM4C123GH6PM.h"
#include "../tm4c123Gh6pm.h"
#include "bsp.h"

/*.........................................................................*/
volatile static uint32_t adcResult = 0;


/* adc1 interrupt handler...................................................*/
void ADC1Seq3_IRQHandler(void) {
	adcResult = ADC1->SSFIFO3;
	ADC1->ISC = (1 << 3);
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
	ADC1->EMUX       =  (0xF<<12);
	ADC1->SSMUX3     =  2;
	ADC1->SSCTL3     =  0x06;
	ADC1->IM         =  (1<<3);
	ADC1->ACTSS     |=  (1<<3);
	ADC1->ISC        =  (1<<3);
	NVIC_EnableIRQ(ADC1SS3_IRQn);
}

/* adc0 initialization - not used...........................................*/
void ADC0_InitSWTriggerSeq3_Ch9(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000030;   // 1) activate clock for Ports E and F
	delay = SYSCTL_RCGC2_R;         //    allow time for clock to stabilize
	GPIOF->DEN       =  0xFF;
	GPIOF->AFSEL     =  0x00;
	GPIOF->DIR       =  0xFF;
	GPIOF->DATA      =  0x02;
	GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE4 input
	GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate function on PE2
	GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
	GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog function on PE2
	SYSCTL_RCGC0_R |= 0x00010000;   // 6) activate ADC0
	delay = SYSCTL_RCGC2_R;
	SYSCTL_RCGC0_R &= ~0x00000300;  // 7) configure for 125K
	ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
	ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
	ADC0_SSMUX3_R += 9;             //    set channel Ain9 (PE4)
	ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
	ADC0_ACTSS_R |= 0x0008;         // 13) enable sample sequencer 3

	NVIC_EnableIRQ(ADC0SS3_IRQn);
}

/* entry point..............................................................*/
int main() {
	SysTick_Init();
	GPIOF_Init();
	GPIOE_Init();
	ADC1_Init();

	while(1) {
		if(adcResult > 2048) {
			GPIOF->DATA |=  (1U << 1);
		} else {
			GPIOF->DATA &= ~(1U << 1);
		}
	}

	return 0;
}
