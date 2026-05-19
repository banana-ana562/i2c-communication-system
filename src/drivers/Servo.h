/*
 * Servo.h
 *
 *	Provides functions to interact with a standard angular Servo Motor
 *
 * Created on: April 23, 2026
 *		Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 */
 
 
#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include "src/drivers/wtimer.h"
//#include "util.h"
#include "src/tm4c123gh6pm.h"

int16_t map(int16_t x, int16_t in_min, int16_t in_max,
            int16_t out_min, int16_t out_max);

// USE HARDWARE PWM MODULE 0 GEN 0 CHANNEL 0 PB7
/* List of Fill In Macros */
#define EN_PWM1_GPIOB_CLOCK		0x02
#define PWM1_PIN							0x80
#define CLEAR_ALT_FUNCTION		0xF0000000
#define PWM1_ALT_FUNCTION			0x40000000
#define EN_PWM1_CLOCK					0x01
#define EN_USE_PWM_DIV				0x00100000
#define CLEAR_PWM_DIV					0x000E0000
#define PWM1_DIV_VALUE				0x000A0000
#define PWM0_DEFAULT_CONFIG		0x00000000
#define PWM0_GEN_CONFIG				0x0000080C
#define PWM1_COUNTER          4999
#define PWM1_START						0x01
#define EN_PWM1_FUNCTION			0x02

/* 16 MHz clock / 64 = 250 kHz PWM clock
 * 20 ms period = 5000 counts
 * 0.5 ms = 125 counts
 * 1.5 ms = 375 counts
 * 2.5 ms = 625 counts
 */

/* 0.5ms (2.5%) - 2.5ms (12.5%) Duty Cycle */
#define SERVO_MIN_CNT           125
#define SERVO_MAX_CNT           625
#define SERVO_CENTER_CNT        375

/* Max Range of Either -90 to 90 or 0 to 180 */
#define SERVO_MIN_ANGLE         -90
#define SERVO_MAX_ANGLE         90

/*
 *	-------------------Servo_Init------------------
 *	Basic Servo Initialization function for PWM Generation
 *	Input: None
 *	Output: None
 */
void Servo_Init(void);

/*
 *	-------------------Drive_Servo------------------
 *	Drives the Servo Motor to a specified angle
 *	Input: Desired Angle
 *	Output: None
 */
void Drive_Servo(int16_t angle);

#endif