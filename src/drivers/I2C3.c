/*
 * I2C3.c
 *
 * Created on: April 23, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Manage all I2C communication with the three slave devices
 * I2C3: SCL - PD0, SDA - PD1
 *
 */
 
#include "src/drivers/I2C3.h"


// Initialize I2C master (I2C1?3, student choice; I2C0 not allowed) at 100 kHz. Enable system clocks, configure
// PD0-1 as I2C alternate function, enable open-drain on SDA, set TPR for 100 kHz
void I2C3_Init(void){
	
	/* Enable Required System Clock */
	SYSCTL_RCGCI2C_R |= EN_I2C3_CLOCK; 	// Enable I2C3 System Clock
	SYSCTL_RCGCGPIO_R |= EN_GPIOD_CLOCK;	// Enable GPIOD System Clock

	while((SYSCTL_PRGPIO_R & EN_GPIOD_CLOCK) == 0){};	//Wait Until GPIOx System Clock is enabled

	/* GPIOx I2C Alternate Function Setup	*/
	GPIO_PORTD_DEN_R |= I2C3_PINS;		//Enable Digital I/O
	GPIO_PORTD_AFSEL_R |= I2C3_PINS;	//Enable Alternate Function Selection

	GPIO_PORTD_PCTL_R &= ~I2C3_ALT_FUNC_MSK;	// Clear existing bits
	GPIO_PORTD_PCTL_R |= I2C3_ALT_FUNC_SET;		//Select I2C3 as the alternate function 	
	GPIO_PORTD_ODR_R |= I2C3_SDA_PIN;					//Enable Open Drain for SDA pin (PD1)
	GPIO_PORTD_AMSEL_R &= ~I2C3_PINS;					//Disable Analog Mode
	
	/*	I2C3 Setup as Master Mode @ 100kBits	*/
	I2C3_MCR_R |= EN_I2C_MASTER;		//Configure I2C2 as Master 
	
	/* Configuring I2C Clock Frequency to 100KHz
		
		TPR = (System Clock / (2*(SCL_LP + SCL_HP) * SCL_CLK)) - 1
		SCL_LP and SCL_HP are fixed
		SCL_LP = 6 & SCL_HP = 4
		
		Example if we want to configure I2C speed to 100kHz for 40MHz system clock
		TPR = (40MHz / ((2*(6+4)) * 100kHz)) - 1 		(Convert Everything to Hz)
		TPR = 19
		
	*/
	
	I2C3_MTPR_R = I2C_MTPR_STD_SPEED | I2C_MTPR_TPR_VALUE;		//TPR = 7 for 100kHz @ 16MHz
	
}

// Send register address, generate repeated START, 
// read one byte. Returns received byte
uint8_t I2C3_Receive(uint8_t slave_addr, uint8_t slave_reg_addr){
    
    // Wait for bus to be free
while(I2C3_MCS_R & I2C_BUSY){};
while(I2C3_MCS_R & I2C_BUS_BUSY){};
	
    // Phase 1: send register address (write)
    I2C3_MSA_R = (slave_addr << 1) | I2C_WRITE;
    I2C3_MDR_R = slave_reg_addr;
    I2C3_MCS_R = I2C_START_RUN;
    while(I2C3_MCS_R & I2C_BUSY){};
    
    // Check for error after write phase
    if(I2C3_MCS_R & I2C_ERROR){
        I2C3_MCS_R = I2C_STOP_RUN;   // force STOP to release bus
        while(I2C3_MCS_R & I2C_BUSY){};
        return 0;
    }

    // Phase 2: repeated START, read one byte
    I2C3_MSA_R = (slave_addr << 1) | I2C_READ;
    I2C3_MCS_R = I2C_START_STOP_RUN;
    while(I2C3_MCS_R & I2C_BUSY){};
    while(I2C3_MCS_R & I2C_BUS_BUSY){};

    if(I2C3_MCS_R & I2C_ERROR){
        return 0;
    }

    return I2C3_MDR_R;
}

// Write one byte to the specified register. Returns 0 on success, non-zero on error
uint8_t I2C3_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t data){
    uint8_t error;


    I2C3_MSA_R = (slave_addr << 1) | I2C_WRITE;
    I2C3_MDR_R = slave_reg_addr;
    I2C3_MCS_R = I2C_START_RUN;

    while(I2C3_MCS_R & I2C_BUSY){}

    if(I2C3_MCS_R & I2C_ERROR){
        I2C3_MCS_R = I2C_STOP_RUN;
        while(I2C3_MCS_R & I2C_BUSY){}
        return I2C3_MCS_R & I2C_ERROR;
    }

    I2C3_MDR_R = data;
    I2C3_MCS_R = I2C_STOP_RUN;

    while(I2C3_MCS_R & I2C_BUSY){}

    error = I2C3_MCS_R & I2C_ERROR;
    return error;
}

// Read multiple consecutive bytes starting at slave_reg_addr into data buffer
void I2C3_Burst_Receive(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size){
    if(size == 0) return;

    while(I2C3_MCS_R & I2C_BUSY){}

    // Write register address
    I2C3_MSA_R = (slave_addr << 1) | I2C_WRITE;
    I2C3_MDR_R = slave_reg_addr;
    I2C3_MCS_R = I2C_START_RUN;

    while(I2C3_MCS_R & I2C_BUSY){}

    if(I2C3_MCS_R & I2C_ERROR){
        I2C3_MCS_R = I2C_STOP_RUN;
        return;
    }

    // Repeated START for read
    I2C3_MSA_R = (slave_addr << 1) | I2C_READ;

    if(size == 1){
        I2C3_MCS_R = I2C_START_STOP_RUN;
        while(I2C3_MCS_R & I2C_BUSY){}
        data[0] = I2C3_MDR_R;
        return;
    }

    I2C3_MCS_R = I2C_START_ACK_RUN;
    while(I2C3_MCS_R & I2C_BUSY){}
    *data++ = I2C3_MDR_R;

    for(int i = 1; i < size - 1; i++){
        I2C3_MCS_R = I2C_ACK_RUN;
        while(I2C3_MCS_R & I2C_BUSY){}
        *data++ = I2C3_MDR_R;
    }

    I2C3_MCS_R = I2C_STOP_RUN;
    while(I2C3_MCS_R & I2C_BUSY){}
    *data = I2C3_MDR_R;
}

// Write multiple consecutive bytes from data buffer. Returns 0 on success
uint8_t I2C3_Burst_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size){
	
	char error;															//Temp Error Variable
	
	/* Asserting Param */
	if (data == 0 || size == 0)
		return 1;
	
	/* Check if I2C3 is busy */
	while(I2C3_MCS_R & I2C_BUSY){};
	
	/* Configure I2C Slave Address, R/W Mode, and what to transmit */
	I2C3_MSA_R = (slave_addr << 1) | I2C_WRITE;	//Slave Address is the first 7 MSB
																					//Clear LSB to write
	I2C3_MDR_R = slave_reg_addr;						//Transmit register addr to interact
	
	/* Initiate I2C by generate a START bit and RUN cmd */
	I2C3_MCS_R = I2C_START_RUN;	// START | RUN
		
	/* Wait until write has been completed */
	while(I2C3_MCS_R & I2C_BUSY){};
		
	/* Loop to Burst Transmit what is stored in data buffer */
	while(size > 1){
		I2C3_MDR_R = *data++;				//Deference Pointer from data array and load into data reg. Post-Increment the pointer after
		I2C3_MCS_R = I2C_RUN;					//Initiate I2C RUN CMD
		while(I2C3_MCS_R & I2C_BUSY){};	//Wait until transmit is complete
		size--;											//Reduce size until 1 is left
		
	}
	
	I2C3_MDR_R = *data;	//Deference Pointer from data array and load into data reg
	I2C3_MCS_R = I2C_STOP_RUN; 	//Initiate I2C STOP condition and RUN CMD
	
	/* Wait until write has been completed */
	while(I2C3_MCS_R & I2C_BUSY){};
	
	/* Wait until bus isn't busy */
	while(I2C3_MCS_R & I2C_BUS_BUSY){};
		
	/* Check for any error */
	error = I2C3_MCS_R & I2C_ERROR;
	return error;
		
}
