/*
 * ButtonLED.h
 *
 *	Provides function to initialize the onboard RGB LED and
 *	buttons on the TIVA TM4C as well as macros for the different
 *	possible color combination 
 *
 * Created on: April 23, 2026
 *		Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 */
 
#ifndef BUTTONLED_H_
#define BUTTONLED_H_

#include "src/tm4c123gh6pm.h"
#include <stdint.h>
#include <stdio.h>

void LED_Init(void);
void BTN_Init(void);

/* Extern declarations so main.c can read the button flags set by the ISR */
extern volatile uint8_t SW1_Flag;
extern volatile uint8_t SW2_Flag;

#define SW1_MASK			0x10
#define SW2_MASK			0x01
#define SWITCH_MASK		0x11
#define LED						(*((volatile uint32_t *)0x40025038))    // Bit addresses for PF1, PF2, PF3
#define LED_MASK			0x0E

#define RED_MASK  		0x02  // PF1
#define GREEN_MASK  	0x08  // PF3
#define BLUE_MASK  		0x04  // PF2

#endif