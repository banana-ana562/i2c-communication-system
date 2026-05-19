// ModuleTest.c
// Course number: 447
// Term: Spring 2026
// Project number: 5 (Final Project)
// Module description: 
// A brief summary on modules tested in this module test file,
// including module list, responsibility and team member assignment
// Team #: 2
// Team members: Ana Arante, Emily Hsu, Suphia Sidiqi


// Header files needed for this program
// Include C library header files, microntroller header files, and 
// project module header files. 
#include "src/drivers/ButtonLED.h"
#include "src/drivers/wtimer.h"
#include "src/drivers/UART0.h"
#include "src/drivers/Servo.h"
#include "src/drivers/MPU6050.h"
#include "src/drivers/TCS34727.h"
#include "src/tm4c123gh6pm.h"
#include <stdio.h>

// preprocessor directives for module name
//#define WTIMER_1MS_TEST		// verify on the oscilloscope
//#define WTIMER_500MS_TEST // verify on the oscilloscope
//#define WTIMER_100MS_TEST // verify on the oscilloscope

//#define SWITCH_TEST
//#define LED_TEST

//#define SERVO_TEST
//#define IMU_TEST
#define TCS34725_TEST

// functions prototypes
void System_Init(void);

// Module test function prototypes
#if defined(WTIMER_1MS_TEST)
	void WTIMER_1MS_Test(void);
#endif

#if defined(WTIMER_500MS_TEST)
	void WTIMER_500MS_Test(void);
#endif

#if defined(WTIMER_100MS_TEST)
	void WTIMER_100MS_Test(void);
#endif

#if defined(SWITCH_TEST)
  void SW1_Test(void);
  void SW2_Test(void);
#endif

#if defined(LED_TEST)
  void LED_Test(void);
#endif

#if defined(SERVO_TEST)
	void Servo_Test(void);
#endif

#if defined(IMU_TEST)
	void IMU_Test(void);
#endif

#if defined(TCS34725_TEST)
    void TCS34725_Test(void);  
#endif

int main(void){
  System_Init();
	
  while(1){
		
		#if defined(WTIMER_1MS_TEST)
			WTIMER_1MS_Test();
		#endif
		
		#if defined(WTIMER_500MS_TEST)
			WTIMER_500MS_Test();
		#endif
		
		#if defined(WTIMER_100MS_TEST)
			WTIMER_100MS_Test();
		#endif

    #if defined(SWITCH_TEST)
      SW1_Test();
      SW2_Test();
    #endif
      
    #if defined(LED_TEST)
      LED_Test();
    #endif
		
		#if defined(SERVO_TEST)
      Servo_Test(); 
		#endif

    #if defined(IMU_TEST)
      IMU_Test();
    #endif
		
		#if defined(TCS34725_TEST)
			TCS34725_Test();
		#endif
  }
}

void System_Init(void){
	
	#if defined(WTIMER_1MS_TEST)
		GPIO_PortE_Init();
		WTIMER1_Init();
	#endif
	
	#if defined(WTIMER_500MS_TEST)
		GPIO_PortE_Init();
		WTIMER1_Init();
	#endif
	
	#if defined(WTIMER_100MS_TEST)
		GPIO_PortE_Init();
		WTIMER1_Init();
	#endif

  #if defined(SWITCH_TEST)
    BTN_Init();
    LED_Init();
    UART0_Init(true,false);
    WTIMER1_Init();
  #endif	
	
	#if defined(LED_TEST)
    BTN_Init();
    LED_Init();
    WTIMER1_Init();
	#endif

  #if defined(SERVO_TEST)
    Servo_Init();
    WTIMER1_Init();
  #endif

  #if defined(IMU_TEST)
    WTIMER1_Init();
    UART0_Init(false, false);   // TX only, no RX interrupt
    I2C3_Init();                // must come before MPU6050_Init
    MPU6050_Init();
	#endif
	
	#if defined(TCS34725_TEST)
    WTIMER1_Init();
    UART0_Init(false, false);   // TX only
		I2C3_Init();
    TCS34725_Init();
	#endif
}

// Functions for WTIMER_TEST
#if defined(WTIMER_1MS_TEST)
void WTIMER_1MS_Test(void) {
    GPIO_PORTE_DATA_R ^= 0x01;   			// toggle PE0
    DELAY_1MS(1);                   // wait 1 ms
}
#endif

#if defined(WTIMER_500MS_TEST)
void WTIMER_500MS_Test(void) {
    GPIO_PORTE_DATA_R ^= 0x01;   			// toggle PE0
    DELAY_1MS(500);                   // wait 500 ms
}
#endif

#if defined(WTIMER_100MS_TEST)
void WTIMER_100MS_Test(void) {
    GPIO_PORTE_DATA_R ^= 0x01;   			// toggle PE0
    DELAY_1MS(100);                  	// wait 100 ms
}
#endif

#if defined(SERVO_TEST)
void Servo_Test(void){
    Drive_Servo(0);     DELAY_1MS(1000);
    Drive_Servo(-45);   DELAY_1MS(1000);
    Drive_Servo(0);     DELAY_1MS(1000);
    Drive_Servo(+45);   DELAY_1MS(1000);
    Drive_Servo(0);     DELAY_1MS(1000);
    Drive_Servo(-90);   DELAY_1MS(1000);
    Drive_Servo(0);     DELAY_1MS(1000);
    Drive_Servo(+90);   DELAY_1MS(1000);
    Drive_Servo(0);     DELAY_1MS(1000);
}
#endif

#if defined(IMU_TEST)
void IMU_Test(void){

    MPU6050_ACCEL_t accel;
    MPU6050_GYRO_t gyro;
    MPU6050_ANGLE_t angle;
    char buf[120];

    MPU6050_Get_Accel(&accel);
    MPU6050_Process_Accel(&accel);

    MPU6050_Get_Gyro(&gyro);
    MPU6050_Process_Gyro(&gyro);

    MPU6050_Get_Angle(&accel, &gyro, &angle);

    sprintf(buf,
            "RAW Ax=%d Ay=%d Az=%d | g Ax=%.2f Ay=%.2f Az=%.2f\r\n",
            accel.Ax_RAW, accel.Ay_RAW, accel.Az_RAW,
            accel.Ax, accel.Ay, accel.Az);
    UART0_OutString(buf);

    sprintf(buf,
            "Angle X=%.2f Y=%.2f Z=%.2f\r\n\r\n",
            angle.ArX, angle.ArY, angle.ArZ);
    UART0_OutString(buf);

    DELAY_1MS(100);   // refresh within 100 ms
}
#endif

#if defined(TCS34725_TEST)
void TCS34725_Test(void){

    RGB_COLOR_HANDLE_t rgb;
    COLOR_DETECTED color;
    char buf[120];

    // TCS34727_GET_RGB internally populates all RAW fields + normalizes R/G/B
    TCS34725_GET_RGB(&rgb);

    // Detect dominant color
    color = Detect_Color(&rgb);

    // Print RAW values
    sprintf(buf,
             "RAW C=%u R=%u G=%u B=%u\r\n",
             rgb.C_RAW, rgb.R_RAW, rgb.G_RAW, rgb.B_RAW);
    UART0_OutString(buf);

    // Print normalized float values
    sprintf(buf,
             "RGB R=%.2f G=%.2f B=%.2f\r\n",
             rgb.R, rgb.G, rgb.B);
    UART0_OutString(buf);
		
		if (color == RED_DETECT){
			UART0_OutString("Detected: RED\r\n\r\n");
		}
		if (color == GREEN_DETECT){
			UART0_OutString("Detected: GREEN\r\n\r\n");
		}
		if (color == BLUE_DETECT){
			UART0_OutString("Detected: BLUE\r\n\r\n");
		}
		if (color == NOTHING_DETECT){
			UART0_OutString("Detected: NOTHING\r\n\r\n");
		}

    DELAY_1MS(500);
}
#endif
