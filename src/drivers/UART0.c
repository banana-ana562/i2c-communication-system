/*
 * UART0.c
 *
 * Created on: April 23, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Send debug information and sensor data to PC terminal
 * 8 data bits, 1 stop bit, no parity, FIFOs enabled
 * Output updates every 1 second (1 Hz)
 *
 */
 
// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1

#include "src/drivers/UART0.h"
#include "src/drivers/MPU6050.h"
#include "src/drivers/TCS34725.h"
#include <stdio.h>

extern RGB_COLOR_HANDLE_t rgb; // TCS34725 color data struct to hold raw and normalized RGB values
extern MPU6050_ANGLE_t angle;

extern char uart_buf[100]; // UART output buffer for sprintf formatting

// Initialize at chosen baud rate (not 57600)
void UART0_Init(void){
	
   SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;
   SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;
 
	while((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOA) != SYSCTL_RCGC2_GPIOA); 
	
	/* GPIO Init */
	GPIO_PORTA_DEN_R 	 |= UART0_PINS;						//Enable Digital Function
	GPIO_PORTA_AFSEL_R |= UART0_PINS;						//Enable Alternate Function
	
	//Set GPIO Alternate Function to UART0
  GPIO_PORTA_AMSEL_R &= ~UART0_PINS;          //Disable Analog Functionality on PB1-0
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
	
	/* UART0 Interrupt Init */
//	UART0_CTL_R &= ~UART_CTL_UARTEN;						//Disable UART Module
	
	UART0_CTL_R &= ~UART_CTL_UARTEN;    // Disable UART before configuring

	UART0_IBRD_R = 8;                    				//IBRD = int(16,000,000 / (16 * 115,200)) = int(8.68)
  UART0_FBRD_R = 44;                     			//FBRD = int(0.68 * 64 + 0.5) = 44
	
	//8 bit word length (no parity bits, one stop bit, FIFO)
	UART0_LCRH_R = UART_LCRH_WLEN_8 | UART_LCRH_FEN;		//Enable RX Interrupt Mask
	
	//UART0_ICR_R |= UART_ICR_RXIC;								//Clear UART RX Interrupt Flag
	//Set UART0 RX Interrupt Priority to 2
	//NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFF1FFF) | 0x00004000;			//Bit 15:13 for UART0 Module
	//NVIC_EN0_R  = NVIC_EN0_UART0;															//Enable UART0 RX Interrupt
	
	//Enable UART
	UART0_CTL_R |= UART_CTL_RXE|UART_CTL_TXE|UART_CTL_UARTEN;	//Enable Tx, RX and UART

	}

// Transmit carriage return + line feed (\r\n)
void UART0_OutCRLF(void){
	  UART0_OutChar(CR);
    UART0_OutChar(LF);
}

// Wait for and return one received ASCII character 
unsigned char UART0_InChar(void){
    while ((UART0_FR_R & UART_FR_RXFE) != 0);   // wait for RX FIFO to have data
    return (unsigned char)(UART0_DR_R & 0xFF);
}

// Transmit a single ASCII character 
void UART0_OutChar(char data){
    while ((UART0_FR_R & UART_FR_TXFF) != 0);   // wait for TX FIFO to have space
    UART0_DR_R = data;
}


// Transmit a null-terminated string
void UART0_OutString(char *pt){
    while (*pt) {           // walk until null terminator
        UART0_OutChar(*pt);
        pt++;
    }
}

void UART0_SendInteger(int32_t n) {
    char buf[16];
    sprintf(buf, "%d", (int)n);   // cast to int to match %d format specifier
    UART0_OutString(buf);
}


void UART0_SendFloat(float f, uint8_t decimals) {
    char buf[32];
    /* sprintf with runtime-precision format string */
    char fmt[8];
    sprintf(fmt, "%%.%df", decimals);    // builds e.g. "%.2f"
    sprintf(buf, fmt, f);
    UART0_OutString(buf);
}


// Receive characters into buffer until 'Enter' or max length
void UART0_InString(char *bufPt, unsigned short max) {
	
	   unsigned short count = 0;
    char ch;
 
    while (1) {
        ch = UART0_InChar();
 
        // Enter key ends input 
        if (ch == CR) {
            break;
        }
		    // Backspace: delete last character if buffer is not empty 
        if ((ch == 0x08 || ch == 0x7F) && count > 0) {
            count--;
            bufPt--;
            UART0_OutChar(0x08);    // move cursor back
            UART0_OutChar(' ');     // erase character on terminal
            UART0_OutChar(0x08);    // move cursor back again
            continue;
        }
				 
        // Only accept printable characters within buffer limit 
        if (count < max && ch >= 0x20 && ch <= 0x7E) {
            *bufPt = ch;
            bufPt++;
            count++;
            UART0_OutChar(ch);      // echo character back to terminal
        }
    }
    // Null-terminate the string 
    *bufPt = '\0';
}

void Print_UART(void){
	sprintf(uart_buf,
					"Color: R=%.0f, G=%.0f, B=%.0f | Angle: X=%d, Y=%d, Z=%d\r\n",
					rgb.R, 
					rgb.G, 
					rgb.B,
					(int)angle.ArX, 
					(int)angle.ArY, 
					(int)angle.ArZ);
	UART0_OutString(uart_buf);
}
		