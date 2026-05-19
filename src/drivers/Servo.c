/*
 * Servo.c
 *
 * Generate hardware PWM on PB4 / M0PWM2 for a standard RC servo
 */

#include "src/drivers/Servo.h"

#define SERVO_PWM_PERIOD   5000   // 20 ms at 16 MHz / 64 = 250 kHz
#define SERVO_MIN_PULSE    250    // 1.0 ms
#define SERVO_CENTER_PULSE 375    // 1.5 ms
#define SERVO_MAX_PULSE    500    // 2.0 ms

void Servo_Init(void){
    // Enable PWM0 and Port B clocks
    SYSCTL_RCGCPWM_R |= 0x01;      // PWM0 clock
    SYSCTL_RCGCGPIO_R |= 0x02;     // Port B clock

    while((SYSCTL_PRGPIO_R & 0x02) == 0){}
    while((SYSCTL_PRPWM_R & 0x01) == 0){}

    // Unlock PB4
    GPIO_PORTB_LOCK_R = 0x4C4F434B;
    GPIO_PORTB_CR_R |= 0x10;

    // Configure PWM clock: system clock / 64
    SYSCTL_RCC_R |= 0x00100000;    // Use PWM divider
    SYSCTL_RCC_R &= ~0x000E0000;   // Clear PWM divider bits
    SYSCTL_RCC_R |=  0x000A0000;   // Divide by 64

    // Configure PB4 as M0PWM2
    GPIO_PORTB_AFSEL_R |= 0x10;        // Enable alternate function on PB4
    GPIO_PORTB_PCTL_R &= ~0x000F0000;  // Clear PB4 PCTL bits
    GPIO_PORTB_PCTL_R |=  0x00040000;  // PB4 = M0PWM2
    GPIO_PORTB_AMSEL_R &= ~0x10;       // Disable analog
    GPIO_PORTB_DEN_R |= 0x10;          // Enable digital

    // Disable PWM generator 1 during setup
    PWM0_1_CTL_R = 0x00;

    // Configure generator 1A for PB4 / M0PWM2
    // Output HIGH on LOAD, LOW on CMPA while counting down
    PWM0_1_GENA_R = 0x0000008C;

    // Set 50 Hz PWM period
    PWM0_1_LOAD_R = SERVO_PWM_PERIOD - 1;

    // Start servo at center position
    PWM0_1_CMPA_R = SERVO_PWM_PERIOD - SERVO_CENTER_PULSE;

    // Enable PWM generator 1
    PWM0_1_CTL_R |= 0x01;

    // Enable M0PWM2 output
    PWM0_ENABLE_R |= 0x04;
}

int16_t map(int16_t x, int16_t in_min, int16_t in_max,
            int16_t out_min, int16_t out_max){
    return (int16_t)((int32_t)(x - in_min) * (out_max - out_min) /
                    (in_max - in_min) + out_min);
}

void Drive_Servo(int16_t angle){
    uint16_t pulse;

    // Clamp angle from -90 to +90 degrees
    if(angle < -90){
        angle = -90;
    }
    if(angle > 90){
        angle = 90;
    }

    // Convert angle to pulse width counts
    pulse = map(angle, -90, 90, SERVO_MIN_PULSE, SERVO_MAX_PULSE);

    // Update compare register
    PWM0_1_CMPA_R = SERVO_PWM_PERIOD - pulse;
}