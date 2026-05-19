/*
 * main.c
 *
 * Full system integration of all 7 modules:
 * - WTIMER1:   1 ms delay used throughout system
 * - UART0:     Serial debug output to PC terminal at 115200 baud
 * - I2C3:      I2C bus communication with all three slave devices
 * - TCS34725:  RGB color sensor at I2C address 0x29
 * - MPU6050:   6-DOF IMU sensor at I2C address 0x68
 * - LCD:       16x2 display with I2C backpack at address 0x27
 * - Servo:     PWM motor control using M0PWM1 on PB7
 */

#include "src/drivers/ButtonLED.h"
#include "src/drivers/wtimer.h"
#include "src/drivers/UART0.h"
#include "src/drivers/I2C3.h"
#include "src/drivers/TCS34725.h"
#include "src/drivers/MPU6050.h"
#include "src/drivers/LCD.h"
#include "src/drivers/Servo.h"
#include "src/tm4c123gh6pm.h"
#include <stdio.h>
#include <stdlib.h>

// Global sensor structs
MPU6050_ACCEL_t accel;
MPU6050_GYRO_t  gyro;
MPU6050_ANGLE_t angle;
RGB_COLOR_HANDLE_t rgb;

char uart_buf[100];

extern void DisableInterrupts(void);
extern void WaitForInterrupt(void);
extern void EnableInterrupts(void);
void System_Init(void);

int main(void){
    System_Init();

		//while(!SW1_Flag){};
	
    /* LCD startup sequence before enabling the servo PWM output */
    LCD_Set_Cursor(ROW1, 0);
    LCD_Print_Str((uint8_t*)"Team");
    LCD_Set_Cursor(ROW2, 0);
    LCD_Print_Str((uint8_t*)"Two");
    DELAY_1MS(1000);
    LCD_Clear();

    /* Initialize servo once after the I2C devices and LCD startup are complete. */
//    Servo_Init();
//    Drive_Servo(0);

    while(1){
			while(!SW1_Flag){};
        /* Refresh sensor values every 100 ms for 1 second total. */
				Get_Color();
				Get_Angle();
				DELAY_1MS(100);

				/* Use one IMU angle axis to control the servo.
				 * Change ArZ to ArY if your physical tilt test is based on Y-axis motion.
				 */
				//DELAY_1MS(100);


        
				
				if(SW2_Flag){
					/* Update LCD and UART once per second. */

					LCD_Clear();
					Print_Color();
					Print_Angle();
					Print_UART();
					SW2_Flag = 0;
				}
    }
}

void System_Init(void){
	SW1_Flag = 0;
	SW2_Flag = 0;
	DisableInterrupts();
	WTIMER1_Init();
	UART0_Init();
	UART0_OutString("System init start\r\n");

	I2C3_Init();
	Servo_Init();
	TCS34725_Init();
	MPU6050_Init();
	UART0_OutString("System init start 2\r\n");


	LED_Init();
	BTN_Init();

	LCD_Init();
	EnableInterrupts();
	UART0_OutString("System init done\r\n");
	
}