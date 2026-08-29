#include <stdint.h>
#include <system.h>
#include "system_time.h"

#define F_CPU    48000000UL
#define TICK_HZ  1000UL


volatile static uint32_t milliseconds = 0;
volatile static uint32_t uptime = 0;

void system_time_init(void) {
    /* CMSIS-provided SysTick_Config(): sets reload value, enables SysTick
       IRQ, enables the counter, clocked from the core clock (48 MHz). */
    SysTick_Config(F_CPU / TICK_HZ);
}

uint32_t millis(void) {
    return milliseconds;
}

uint32_t get_uptime(void) {
    return uptime;
}

void SysTick_Handler(void)
{
    milliseconds++;
    if ((milliseconds % 1000) == 0) {
        uptime++;    
    }
}
