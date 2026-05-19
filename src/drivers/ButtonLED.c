/*
 * ButtonLED.c
 *
 * Created on: April 23, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Red LED blinks at 2 Hz using DELAY_1MS(500) in a loop
 * SW1 press toggles between UART test mode and WTIMER delay mode
 * SW2 press (in timer mode) cycles LED color: RED -> GREEN -> BLUE -> RED ?
 *
 */
 
#include "src/drivers/ButtonLED.h"
#include "src/drivers/UART0.h"
#include "src/drivers/MPU6050.h"
#include "src/drivers/TCS34725.h"
#include "src/drivers/LCD.h"

volatile uint8_t SW1_Flag = 0;  // set when SW1 (PF4) is pressed -> start/stop
volatile uint8_t SW2_Flag = 0;  // set when SW2 (PF0) is pressed -> display update


// Initialize Port F GPIO for LED output on PF1-3
void LED_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x20;  // Port F
	while((SYSCTL_PRGPIO_R & 0x20) == 0){};	
  
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock GPIO Port F
  GPIO_PORTF_CR_R |= LED_MASK;      // allow changes to PF4,0
  GPIO_PORTF_DIR_R |= LED_MASK;     // make PA5 out
	GPIO_PORTF_AFSEL_R &= ~LED_MASK;  // disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= LED_MASK;     // enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~LED_MASK;  // disable analog functionality on PF4,0
}

// Initialize Port F GPIO for button input on PF4 and PF0
// Configure pull-up resistors for SW1 and SW2
void BTN_Init(void){
  volatile unsigned long delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;	// Activate F clocks
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOF)==0){};
		
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // unlock PortF PF0  
	GPIO_PORTF_CR_R |= 0x11; // allow changes to PF4 :10000->0x10
	GPIO_PORTF_AMSEL_R &= ~0x11; // disable analog function
	GPIO_PORTF_PCTL_R &= ~0x000F000F; // GPIO clear bit PCTL
	GPIO_PORTF_PUR_R |= 0x11; // enable pull upresistor on pf4
	GPIO_PORTF_DIR_R &= ~0x11; // PF4 input
	GPIO_PORTF_AFSEL_R &= ~0x11; // no alternate function
	GPIO_PORTF_DEN_R |= 0x11; // enable digital pins PF0 and PF4
	GPIO_PORTF_IS_R &= ~0x11;
	GPIO_PORTF_IBE_R &= ~0x11; 	// PF4 is edge-sensitive
	GPIO_PORTF_IEV_R &= ~0x11;	// PF4 is not both edges
	GPIO_PORTF_ICR_R |= 0x11;	// PF4 rising edge event
	GPIO_PORTF_IM_R |= 0x11;	// Clear Flag
														// Arm interrupt on PF0 and PF4
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFFFFFF00) | (1 << 5);
	NVIC_EN0_R = 0x40000000; // (h) enable Port F edge interrupt
}

void GPIOPortF_Handler(void){
			for (uint32_t time = 0; time < 160000; time++){}

	
    /* Check if SW1 (PF4) triggered the interrupt */
    if(GPIO_PORTF_RIS_R & SW1_MASK){
        SW1_Flag = 1;                   // signal main() that SW1 was pressed
        GPIO_PORTF_ICR_R |= SW1_MASK;   // clear the interrupt flag for PF4
    }

    /* Check if SW2 (PF0) triggered the interrupt */
    if(GPIO_PORTF_RIS_R & SW2_MASK){
        SW2_Flag = 1;                   // signal main() that SW2 was pressed
//				if(SW2_Flag){
					/* Update LCD and UART once per second. */
				//	SW2_Flag = 0;
			//	}

        GPIO_PORTF_ICR_R |= SW2_MASK;   // clear the interrupt flag for PF0
    }
}
