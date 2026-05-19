/*
 * wtimer.h
 *
 *	Provides basic utility function such as delay and 
 *	value mapping
 *
 * Created on: April 23, 2026
 *		Authors: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 */
 
#ifndef WTIMER_H_
#define WTIMER_H_
#include <stdint.h>
#include "src/tm4c123gh6pm.h"

//#define CONSTANT_FILL       (0)
//#define CODE_FILL           (0)

#define EN_WTIMER_CLOCK     (0x02)
#define WTIMER_TAEN_BIT     (0x00000001)
#define WTIMER_32_BIT_CFG   (0x00000000)
#define WTIMER_PERIOD_CD    (0x00000002)
#define WTIMER_TATORIS_BIT  (0x00000001)
#define PRESCALER_VALUE     (0)
#define RELOAD_VALUE        (15999)

void WTIMER1_Init(void);
void GPIOPortE_Init(void);
void DELAY_1MS(uint32_t ms);

///*
//* Maps a value from one range to another. Used to map an angle in degrees to a PWM compare value.
//*/
//static int16_t map(int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max){
//    return (x - in_min) * (out_max - out_min) /
//           (in_max - in_min) + out_min;
//}

#endif