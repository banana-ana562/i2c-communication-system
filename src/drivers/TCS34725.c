/*
 * TCS34725.c
 *
 * Created on: April 23, 2026
 * Author: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Detect the dominant color of objects using raw RGB and clear data
 * Uses I2C1: SCL - PA6, SDA - PA7
 *
 */

#include "src/drivers/TCS34725.h"
#include "src/drivers/I2C3.h"
#include "src/drivers/LCD.h"
#include "src/drivers/ButtonLED.h"

COLOR_DETECTED color;
extern 	RGB_COLOR_HANDLE_t rgb;


// Verify device ID; set integration time to 2.4 ms; set gain to 1×; power on sensor; enable RGBC. 
// Prints status to UART0 at each step.
void TCS34725_Init(void){
	uint8_t ret;																//Temp Variable to hold return values
	char printBuf[20];													//String buffer to print
	
	/* Check if RGB Color Sensor has been detected */
	ret = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_ID_R_ADDR);

	//Print ID or Error to PC Serial Terminal
	if(ret != TCS34725_ID){
		UART0_OutString("TCS34725 has not been Detected\r\n");
		return;
	}
	UART0_OutString("TCS34725 has been Detected\r\n");
	
	/* Set Integration Time to 2.4ms in timing register */
	ret = I2C3_Transmit(TCS34725_ADDR, TCS34725_CMD | TCS34725_TIMING_R_ADDR, TCS34725_ATIME_2_4_MS);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34725 Integration Time Set\r\n");
	
	// Necessary Delay when setting integration time/wait time. 
	// This varies for which integration time is choosen.
	// This project chooses 2.4ms.
	DELAY_1MS(3);
	
	/* Setting Gain to 1X gain */
	ret = I2C3_Transmit(TCS34725_ADDR, TCS34725_CMD | TCS34725_CTRL_R_ADDR, TCS34725_CTRL_AGAIN_1);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34725 Gain Set\r\n");
	
	/* Powering On Sensor at Enable register */
	ret = I2C3_Transmit(TCS34725_ADDR, TCS34725_CMD | TCS34725_ENABLE_R_ADDR, TCS34725_ENABLE_PON);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34725 Power On\r\n");

	//Nessessary Delay When Powering On Module
	DELAY_1MS(3);
	
	/* Enabling RGBC 2-Channel ADC at Enable register */
	ret = I2C3_Transmit(TCS34725_ADDR, TCS34725_CMD | TCS34725_ENABLE_R_ADDR, TCS34725_ENABLE_PON|TCS34725_ENABLE_AEN);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34725 RGBC On\r\n");
	
	//Integration Time Delay when Activating. Varies with Integration Time Choosen by User
	DELAY_1MS(3);
	
	UART0_OutString("TCS34725 Color Sensor Initialized\r\n");
	
}

// Read and return 16-bit clear channel data (CDATAL + CDATAH)
uint16_t TCS34725_GET_RAW_CLEAR(void){
	uint8_t CLEAR_LOW;
	uint8_t CLEAR_HIGH;
	uint16_t CLEAR_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	CLEAR_LOW = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_CDATAL_R_ADDR);
	CLEAR_HIGH = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_CDATAH_R_ADDR);
	
	/* Concatanate into 16-bit value */
	CLEAR_DATA = (uint16_t)((CLEAR_HIGH << 8) | (CLEAR_LOW));
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return CLEAR_DATA;
}

// Read and return 16-bit red channel data (RDATAL + RDATAH)
uint16_t TCS34725_GET_RAW_RED(void){
	uint8_t RED_LOW;
	uint8_t RED_HIGH;
	uint16_t RED_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	RED_LOW = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_RDATAL_R_ADDR);
	RED_HIGH = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_RDATAH_R_ADDR);

	/* Concatanate into 16-bit value */
	RED_DATA = (uint16_t)((RED_HIGH << 8) | (RED_LOW));
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return RED_DATA;
}

// Read and return 16-bit green channel data (GDATAL + GDATAH)

uint16_t TCS34725_GET_RAW_GREEN(void){
	uint8_t GREEN_LOW;
	uint8_t GREEN_HIGH;
	uint16_t GREEN_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	GREEN_LOW = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_GDATAL_R_ADDR);
	GREEN_HIGH = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_GDATAH_R_ADDR);

	/* Concatanate into 16-bit value */
	GREEN_DATA = (uint16_t)((GREEN_HIGH << 8) | (GREEN_LOW));
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return GREEN_DATA;
}

// Read and return 16-bit blue channel data (BDATAL + BDATAH)
uint16_t TCS34725_GET_RAW_BLUE(void){
	uint8_t BLUE_LOW;
	uint8_t BLUE_HIGH;
	uint16_t BLUE_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	BLUE_LOW = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_BDATAL_R_ADDR);
	BLUE_HIGH = I2C3_Receive(TCS34725_ADDR, TCS34725_CMD | TCS34725_BDATAH_R_ADDR);

	/* Concatanate into 16-bit value */
	BLUE_DATA = (uint16_t)((BLUE_HIGH << 8) | (BLUE_LOW));
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return BLUE_DATA;
}

// Populate inst?R_RAW / G_RAW / B_RAW / C_RAW from sensor
// normalize R/G/B to 0–255 by dividing by C_RAW. Guard against division by zero.
void TCS34725_GET_RGB(RGB_COLOR_HANDLE_t* RGB_COLOR_Instance){
	
	/* Populate RAW values from sensor */
	RGB_COLOR_Instance -> R_RAW = TCS34725_GET_RAW_RED();
	RGB_COLOR_Instance -> G_RAW = TCS34725_GET_RAW_GREEN();
	RGB_COLOR_Instance -> B_RAW = TCS34725_GET_RAW_BLUE();
	RGB_COLOR_Instance -> C_RAW = TCS34725_GET_RAW_CLEAR();
	
	/* Prevent Dividing by 0 by checking if the C_RAW value from struct is equal to 0 */
	if(RGB_COLOR_Instance -> C_RAW == 0)
		return;
	
	/*
	Divide all RGB value with their (RAW Value / (float)Clear Raw Value) and multiple everything with 255.0
	Store in RGB Color Instance Struct
	*/ 
	RGB_COLOR_Instance -> R = (RGB_COLOR_Instance -> R_RAW / (float)RGB_COLOR_Instance -> C_RAW) * 255.0;
	RGB_COLOR_Instance -> G = (RGB_COLOR_Instance -> G_RAW / (float)RGB_COLOR_Instance -> C_RAW) * 255.0;
	RGB_COLOR_Instance -> B = (RGB_COLOR_Instance -> B_RAW / (float)RGB_COLOR_Instance -> C_RAW) * 255.0;
	
}

// Compare normalized R, G, B values; return the dominant COLOR_DETECTED enum. 
// Returns NOTHING_DETECT if no clear dominant color.
COLOR_DETECTED Detect_Color(RGB_COLOR_HANDLE_t* RGB_COLOR_Instance){
	
	float R = RGB_COLOR_Instance -> R;
	float G = RGB_COLOR_Instance -> G;
	float B = RGB_COLOR_Instance -> B;
	
	/* Compare all values with eachother and return which color is prominent using enum type */
	if((R > G) && (R > B)){
		return RED_DETECT;
	} else if ((G > R) && (G > B)){
		return GREEN_DETECT;
	} else if ((B > R) && (B > G)){
		return BLUE_DETECT;
	}
	
	/* Otherwise no color is being detected */
	return NOTHING_DETECT;
}

void Get_Color(void){
    char buf[120];

    // TCS34727_GET_RGB internally populates all RAW fields + normalizes R/G/B
    TCS34725_GET_RGB(&rgb);

    // Detect dominant color
    color = Detect_Color(&rgb);
}

void Print_Color(void){
		char buf[120];
		
		LCD_Set_Cursor(ROW1, 0);

		if (color == RED_DETECT){
			LCD_Print_Str((uint8_t*)"Color: RED");
			LED &= ~LED_MASK;
			LED |= RED_MASK;
			
		}
		if (color == GREEN_DETECT){
			LCD_Print_Str((uint8_t*)"Color: GREEN");
			LED &= ~LED_MASK;
			LED |= GREEN_MASK;

		}
		if (color == BLUE_DETECT){
			LCD_Print_Str((uint8_t*)"Color: BLUE");
			LED &= ~LED_MASK;
			LED |= BLUE_MASK;

		}
		if (color == NOTHING_DETECT){
			LCD_Print_Str((uint8_t*)"Color: NOTHING");
			LED &= ~LED_MASK;

		}

    //DELAY_1MS(1000);
		//LCD_Clear();

}
