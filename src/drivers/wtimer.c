/*
 * wtimer.c
 *
 * Created on: April 23, 2026
 * Author: Ana Arante, Emily Hsu, Suphia Sidiqi
 *
 * Provide 1 millisecond delay function used by all other 
 * modules; general-purpose value mapping
 *
 */
 
//#include "util.h"
#include "src/drivers/wtimer.h"

/* Local Macros */
#define TIMER_32_MAX_RELOAD		(0)	
 
/* The reason why Wide Timer is used instead of regular time is because
	 of the prescaler option */
	 
void WTIMER1_Init(void){
	SYSCTL_RCC_R |= 0x00100000;
	SYSCTL_RCC_R  = (SYSCTL_RCC_R & ~0x000E0000) | 0x000A0000;

	SYSCTL_RCGCWTIMER_R |= EN_WTIMER_CLOCK;
	while((SYSCTL_RCGCWTIMER_R & EN_WTIMER_CLOCK) != EN_WTIMER_CLOCK){};
	WTIMER1_CTL_R  &= ~WTIMER_TAEN_BIT;
	WTIMER1_CFG_R   = WTIMER_32_BIT_CFG;
	WTIMER1_TAMR_R  = WTIMER_PERIOD_CD;
	WTIMER1_TAPR_R  = PRESCALER_VALUE;
	WTIMER1_TAILR_R = RELOAD_VALUE;
	WTIMER1_ICR_R   = WTIMER_TATORIS_BIT;  // clear flag ? correct bit
	WTIMER1_CTL_R  |= WTIMER_TAEN_BIT;     // start ? runs continuously
}

void DELAY_1MS(uint32_t ms){
    uint32_t i;
    for(i = 0; i < ms; i++){
        WTIMER1_ICR_R = WTIMER_TATORIS_BIT;               // clear timeout flag
        while((WTIMER1_RIS_R & WTIMER_TATORIS_BIT) == 0); // poll until 1ms elapsed
    }
}
	 