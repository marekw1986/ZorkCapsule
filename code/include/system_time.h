#ifndef _SYSTEM_TIME_H_
#define _SYSTEM_TIME_H_

#include <stdint.h>

void system_time_init(void);
uint32_t millis (void);
uint32_t get_uptime(void);

#endif // _SYSTEM_TIME_H_
