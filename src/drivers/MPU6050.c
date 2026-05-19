/*
 * MPU6050.c
 *
 * Created on: April 23, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Measure 3-axis acceleration and 3-axis rotation rate; compute tilt angles
 *
 */
 
 
#include "src/drivers/MPU6050.h"
#include "src/drivers/wtimer.h" // for DELAY_1MS used in initializing
#include "src/drivers/LCD.h"
#include "src/drivers/Servo.h"

#define ACCEL_LSB_0_VALUE		(16384.0) 
#define ACCEL_LSB_1_VALUE		(8192.0)
#define ACCEL_LSB_2_VALUE		(4096.0)
#define ACCEL_LSB_3_VALUE		(2048.0)

#define GYRO_LSB_0_VALUE		(131.0)
#define GYRO_LSB_1_VALUE		(65.5)
#define GYRO_LSB_2_VALUE		(32.8)
#define GYRO_LSB_3_VALUE		(16.4)

	char buf[120];
extern	MPU6050_ACCEL_t accel;
extern	MPU6050_GYRO_t gyro;
extern	MPU6050_ANGLE_t angle;


// Read WHO_AM_I, print ID or error to UART0, reset device
// wake up sensor, set sample rate to 1 kHz, apply default config/accel/gyro settings
void MPU6050_Init(void){
	
	uint8_t ret;
	char stringBuf[15];
	
	//Check device ID if it does not equal to MPU's ID, MPU is not detected
	ret = I2C3_Receive(MPU6050_ADDR_AD0_LOW, WHO_AM_I);	// Read WHO_AM_I register

	if (ret != MPU6050_ADDR_AD0_LOW){
		UART0_OutString("MPU6050 not detected\r\n");
		return;
	}
	
	//Print device ID out to terminal if device is detected
	UART0_OutString("MPU6050 detected\r\n");
	sprintf(stringBuf, "MPU ID = 0x%X\r\n", ret);
	UART0_OutString(stringBuf);
	
	/* Reset the MPU6050 Module */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, PWR_MGMT_1, PWR_DEVICE_RESET);	//Write 0x80 to PWR_MGMT_1 to reset device
	DELAY_1MS(100); // add a delay of 100ms to ensure reset
	
	/* 0 to wake up sensor */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, PWR_MGMT_1, PWR_CLK_SEL_INTERNAL);	//Write 0x00 to PWR_MGMT_1 to wake up sensor
	DELAY_1MS(100); // add a delay of 100ms to ensure wake up is complete before proceeding with configuration

	/* Set Data Rate to 1kHz */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, SMPLRT_DIV, SMPLRT_DIV_8); // if off then try SMPLRT_DIV = 0, Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV) = 8kHz / (1 + 0) = 8kHz
	
	/* Default Configuration */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, CONFIG, CONFIG_DFPL_0);	//Default Config: Set DLPF to 0 (no digital low pass filter)
	
	/* Default config for Accelerometer */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, ACCEL_CONFIG, ACCEL_AFS_SEL_0);	//Default Accel Config: Set AFS_SEL to 0 (±2 g)
	
	/* Default config for Gyroscope */
	I2C3_Transmit(MPU6050_ADDR_AD0_LOW, GYRO_CONFIG, GYRO_FS_SEL_0);	//Default Gyro Config: Set FS_SEL to 0 (±250 °/s)
}

// Read all 6 raw accelerometer bytes (ACCEL_XOUT_H/L through ACCEL_ZOUT_H/L)
// via I2C, assemble 16-bit signed values and store in struct
void MPU6050_Get_Accel(MPU6050_ACCEL_t* Accel_Instance){
	
	/* Local Variables */
	uint8_t ACCEL_X_LOW;
	uint8_t ACCEL_X_HIGH;
	uint8_t ACCEL_Y_LOW;
	uint8_t ACCEL_Y_HIGH;
	uint8_t ACCEL_Z_LOW;
	uint8_t ACCEL_Z_HIGH;
	
	/* Grab 16-bit Accel data of each axis by reading ACCEL data register using I2C*/
ACCEL_X_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_XOUT_H);
ACCEL_X_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_XOUT_L);
ACCEL_Y_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_YOUT_H);
ACCEL_Y_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_YOUT_L);
ACCEL_Z_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_ZOUT_H);
ACCEL_Z_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_ZOUT_L);
	
	/* Concatenate and Save Into Accelerometer Struct Instance */
	// Shift high byte to the left by 8 bits to make room for low byte, then OR with low byte to get full 16-bit value. Cast to int16_t to interpret as signed value
	Accel_Instance->Ax_RAW = (int16_t)((ACCEL_X_HIGH << 8) | ACCEL_X_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
	Accel_Instance->Ay_RAW = (int16_t)((ACCEL_Y_HIGH << 8) | ACCEL_Y_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
	Accel_Instance->Az_RAW = (int16_t)((ACCEL_Z_HIGH << 8) | ACCEL_Z_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
}

// Read all 6 raw gyroscope bytes (GYRO_XOUT_H/L through GYRO_ZOUT_H/L)
// via I2C, assemble 16-bit signed values and store in struct
void MPU6050_Get_Gyro(MPU6050_GYRO_t* Gyro_Instance){
		
	/* Local Variables */
	uint8_t GYRO_X_LOW;
	uint8_t GYRO_X_HIGH;
	uint8_t GYRO_Y_LOW;
	uint8_t GYRO_Y_HIGH;
	uint8_t GYRO_Z_LOW;
	uint8_t GYRO_Z_HIGH;
	
	/* Grab 16-bit Gyro Data of each Axis by reading GYRO data register using I2C*/
GYRO_X_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_XOUT_H);
GYRO_X_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_XOUT_L);
GYRO_Y_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_YOUT_H);
GYRO_Y_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_YOUT_L);
GYRO_Z_HIGH = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_ZOUT_H);
GYRO_Z_LOW  = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_ZOUT_L);	
	/* Concatanate and Save Into Gyro Struct Instance */
	// Shift high byte to the left by 8 bits to make room for low byte, then OR with low byte to get full 16-bit value. Cast to int16_t to interpret as signed value
	Gyro_Instance->Gx_RAW = (int16_t)((GYRO_X_HIGH << 8) | GYRO_X_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
	Gyro_Instance->Gy_RAW = (int16_t)((GYRO_Y_HIGH << 8) | GYRO_Y_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
	Gyro_Instance->Gz_RAW = (int16_t)((GYRO_Z_HIGH << 8) | GYRO_Z_LOW); // Shift high byte to the left by 8 bits and OR with low byte to get full 16-bit value
}

// Read ACCEL_CONFIG register to determine active LSB sensitivity
// divide RAW values by the appropriate ACCEL_LSB_x_VALUE, store result in Ax/Ay/Az
void MPU6050_Process_Accel(MPU6050_ACCEL_t* Accel_Instance){
	
	char LSB_Sensitivity;
	
	//Read LSB Sensitivity Setting from ACCEL_CONFIG Register
	LSB_Sensitivity = I2C3_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_CONFIG);
	
	//Based on setting, process raw data accordingly
	LSB_Sensitivity = (LSB_Sensitivity & ACCEL_AFS_SEL_3); // Keep AFS_SEL bits 4:3

	if (LSB_Sensitivity == ACCEL_AFS_SEL_0){ // If AFS_SEL is 0, LSB sensitivity is 16384 LSB/g
		Accel_Instance->Ax = Accel_Instance->Ax_RAW / ACCEL_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Ay = Accel_Instance->Ay_RAW / ACCEL_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Az = Accel_Instance->Az_RAW / ACCEL_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
	}
	else if (LSB_Sensitivity == ACCEL_AFS_SEL_1){ // If AFS_SEL is 1, LSB sensitivity is 8192 LSB/g
		Accel_Instance->Ax = Accel_Instance->Ax_RAW / ACCEL_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Ay = Accel_Instance->Ay_RAW / ACCEL_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Az = Accel_Instance->Az_RAW / ACCEL_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
	}
	else if (LSB_Sensitivity == ACCEL_AFS_SEL_2){ // If AFS_SEL is 2, LSB sensitivity is 4096 LSB/g
		Accel_Instance->Ax = Accel_Instance->Ax_RAW / ACCEL_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Ay = Accel_Instance->Ay_RAW / ACCEL_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Az = Accel_Instance->Az_RAW / ACCEL_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
	}
	else { // If AFS_SEL is 3, LSB sensitivity is 2048 LSB/g
		Accel_Instance->Ax = Accel_Instance->Ax_RAW / ACCEL_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Ay = Accel_Instance->Ay_RAW / ACCEL_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
		Accel_Instance->Az = Accel_Instance->Az_RAW / ACCEL_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get acceleration in g
	}
}

// Divide RAW gyroscope values by the appropriate GYRO_LSB_x_VALUE; store result in Gx/Gy/Gz
void MPU6050_Process_Gyro(MPU6050_GYRO_t* Gyro_Instance){

	char LSB_Sensitivity;

	// Read LSB Sensitivity Setting from GYRO_CONFIG Register
	LSB_Sensitivity = I2C3_Receive(MPU6050_ADDR_AD0_LOW, GYRO_CONFIG);

	// Based on setting, process raw data accordingly
	LSB_Sensitivity = (LSB_Sensitivity & GYRO_FS_SEL_3); // Keep only FS_SEL bits 4:3

	if (LSB_Sensitivity == GYRO_FS_SEL_0){ // If FS_SEL is 0, LSB sensitivity is 131 LSB/°/s
		Gyro_Instance->Gx = Gyro_Instance->Gx_RAW / GYRO_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gy = Gyro_Instance->Gy_RAW / GYRO_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gz = Gyro_Instance->Gz_RAW / GYRO_LSB_0_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
	}
	else if (LSB_Sensitivity == GYRO_FS_SEL_1){ // If FS_SEL is 1, LSB sensitivity is 65.5 LSB/°/s
		Gyro_Instance->Gx = Gyro_Instance->Gx_RAW / GYRO_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gy = Gyro_Instance->Gy_RAW / GYRO_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gz = Gyro_Instance->Gz_RAW / GYRO_LSB_1_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
	}
	else if (LSB_Sensitivity == GYRO_FS_SEL_2){ // If FS_SEL is 2, LSB sensitivity is 32.8 LSB/°/s
		Gyro_Instance->Gx = Gyro_Instance->Gx_RAW / GYRO_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gy = Gyro_Instance->Gy_RAW / GYRO_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gz = Gyro_Instance->Gz_RAW / GYRO_LSB_2_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
	}
	else { // If FS_SEL is 3, LSB sensitivity is 16.4 LSB/°/s
		Gyro_Instance->Gx = Gyro_Instance->Gx_RAW / GYRO_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gy = Gyro_Instance->Gy_RAW / GYRO_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
		Gyro_Instance->Gz = Gyro_Instance->Gz_RAW / GYRO_LSB_3_VALUE; // Divide raw value by LSB sensitivity to get rotation rate in °/s
	}
}

// Use atan2() with processed Accel values and RAD_TO_DEGREE_CONV to compute ArX, ArY, ArZ in degrees
void MPU6050_Get_Angle(MPU6050_ACCEL_t* Accel_Instance, MPU6050_GYRO_t* Gyro_Instance, MPU6050_ANGLE_t* Angle_Instance){
	
	//Hint: Use RAD_TO_DEGREE_CONV macro to convert radian to degree
	Angle_Instance->ArX = atan2(Accel_Instance->Ay, Accel_Instance->Az) * RAD_TO_DEGREE_CONV; // Compute ArX using atan2 of Ay and Az, then convert to degrees

	Angle_Instance->ArY = atan2(-Accel_Instance->Ax,
                                sqrt((Accel_Instance->Ay * Accel_Instance->Ay) +
                                     (Accel_Instance->Az * Accel_Instance->Az))) * RAD_TO_DEGREE_CONV; // Compute ArY using atan2 of -Ax and the square root of (Ay^2 + Az^2), then convert to degrees

    Angle_Instance->ArZ = atan2(Accel_Instance->Az, Accel_Instance->Ax) * RAD_TO_DEGREE_CONV;	// Compute ArZ using atan2 of Az and Ax, then convert to degrees
}

// Read a single register for debugging purposes (returns raw byte)
uint8_t MPU6050_Read_Reg(uint8_t reg){
	return I2C3_Receive(MPU6050_ADDR_AD0_LOW, reg); // Read the specified register using I2C and return the raw byte
}

void Get_Angle(void){
	MPU6050_Get_Accel(&accel);
	MPU6050_Process_Accel(&accel);

	MPU6050_Get_Gyro(&gyro);
	MPU6050_Process_Gyro(&gyro);

	MPU6050_Get_Angle(&accel, &gyro, &angle);
	
	int16_t servo_angle = angle.ArY;

	Drive_Servo(servo_angle);

}

void Print_Angle(void){
	char buf[120];

	LCD_Set_Cursor(ROW2, 0);
	sprintf(buf,
		"Angle: %d", angle.ArY);
	LCD_Print_Str((uint8_t*)buf);

	//DELAY_1MS(1000);   // refresh within 100 ms
	//LCD_Clear();

}

