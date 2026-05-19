/*
 * I2C.h
 *
 *	Provides the I2C Init, Read, and Write Function
 *
 * Created on: April 23, 2026
 *		Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "src/tm4c123gh6pm.h"
#include "src/drivers/wtimer.h"

/* List of Fill In Macros */
// I2C3: SCL - PD0, SDA - PD1
#define EN_I2C3_CLOCK				0x08
#define EN_GPIOD_CLOCK			0x08
#define I2C3_PINS						0x03				// PD0-1
#define I2C3_ALT_FUNC_MSK		0x000000FF	// clears PCTL bits for PD0-1
#define I2C3_ALT_FUNC_SET		0x00000033	// sets function 3 for PD0-1
#define I2C3_SDA_PIN				0x02 				// PD1
#define I2C3_SCL_PIN				0x01				// PD0

// Shared
#define EN_I2C_MASTER				0x10
#define I2C_MTPR_TPR_VALUE	0x07
#define I2C_MTPR_STD_SPEED 	0x00

//Transmit Function (Most came from above Macros)
#define I2C_RW_PIN					0x01

//Burst Transmit Function
#define RUN_CMD							0x01

// I2C MCS Command Bits
#define I2C_RUN 						0x01	// RUN
#define I2C_START_RUN				0x03	// START | RUN
#define I2C_STOP_RUN				0x05	// STOP | RUN
#define I2C_START_STOP_RUN	0x07 	// START | STOP | RUN
#define I2C_ACK_RUN					0x09	// ACK | RUN
#define I2C_START_ACK_RUN	0x0B	// START | ACK | RUN

// I2C MCS Status Bits	
#define I2C_BUSY						0x01	// Master busy
#define I2C_ERROR						0x02	// Error flag
#define I2C_BUS_BUSY				0x40	// Bus busy

// I2C MSA Bits
#define I2C_WRITE						0x00	// Clear LSB to write
#define I2C_READ						0x01	// Set LSB to read


/*
 *	-------------------I2C3_Init------------------
 *	Basic I2C Initialization function for master mode @ 100kHz
 *	Input: None
 *	Output: None
 */
void I2C3_Init(void);

/*
 *	-------------------I2C3_Receive------------------
 *	Polls to receive data from specified peripheral
 *	Input: Slave address & Slave Register Address
 *	Output: Returns 8-bit data that has been received
 */
uint8_t I2C3_Receive(uint8_t slave_addr, uint8_t slave_reg_addr);

/*
 *	-------------------I2C3_Transmit------------------
 *	Transmit a byte of data to specified peripheral
 *	Input: Slave address, Slave Register Address, Data to Transmit
 *	Output: Any Errors if detected, otherwise 0
 */
uint8_t I2C3_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t data);

uint8_t I2C3_Transmit_Byte(uint8_t slave_addr, uint8_t data);

//Has Yet to be Implemented
/*
 *	----------------I2C3_Burst_Receive-----------------
 *	Polls to receive multiple bytes of data from specified
 *  peripheral by incrementing starting slave register address
 *	Input: Slave address, Slave Register Address, Data Buffer
 *	Output: None
 */
void I2C3_Burst_Receive(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size);

/*
 *	----------------I2C3_Burst_Transmit-----------------
 *	Transmit multiple bytes of data to specified peripheral
 *  by incrementing starting slave address
 *	Input: Slave address, Slave Register Address, Data Buffer to transmit, Size of Transmit
 *	Output: None
 */
uint8_t I2C3_Burst_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size);

#endif //I2C_H_


