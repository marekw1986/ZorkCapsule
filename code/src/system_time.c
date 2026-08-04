#include <stdint.h>
#include <system.h>
#include "system_time.h"

#define F_CPU    48000000UL
#define TICK_HZ  1000UL


volatile static uint32_t milliseconds = 0;

void system_time_init(void) {
    /* CMSIS-provided SysTick_Config(): sets reload value, enables SysTick
       IRQ, enables the counter, clocked from the core clock (48 MHz). */
    SysTick_Config(F_CPU / TICK_HZ);
}

uint32_t millis(void) {
    return milliseconds;
}

void SysTick_Handler(void)
{
    milliseconds++;
}
