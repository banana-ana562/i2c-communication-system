/*
 * Servo_Test.c
 *
 * Created on: April 23, 2026
 * Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Generate hardware PWM to position a standard RC servo motor
 *
 */

#include "src/drivers/Servo.h"
#include "src/drivers/wtimer.h" // for DELAY_1MS used in testing
#include "src/modules/Servo_Test.h" // for Servo_Test()

#if defined(SERVO_TEST)
void Servo_Test(void){
    // Test code for Servo module
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