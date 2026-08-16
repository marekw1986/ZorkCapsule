#ifndef _LOCK_H_
#define _LOCK_H_

#include <stdint.h>

void initialize_pwm(void);;
void open_lock(void);
void handle_lock(void);
void set_lantern_indicator(uint8_t val);
void set_sword_indicator(uint8_t val);
void set_score_indicator(uint8_t val);
uint8_t map_u8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max);

#endif // _LOCK_H_
