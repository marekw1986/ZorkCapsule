#include <tcc.h>
#include "box_control.h"

struct tcc_module tcc0_instance;
struct tcc_module tcc2_instance;

void initialize_pwm(void)
{
    struct tcc_config config_tcc0;
    tcc_get_config_defaults(&config_tcc0, TCC0);
    config_tcc0.counter.period = 255;
    config_tcc0.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
    config_tcc0.compare.match[0] = 0;   /* LED1 duty, CC0 -> WO[4] */
    config_tcc0.compare.match[1] = 0;   /* LED2 duty, CC1 -> WO[5] */
    config_tcc0.pins.enable_wave_out_pin[4] = true;
    config_tcc0.pins.wave_out_pin[4]        = PIN_PA14F_TCC0_WO4;
    config_tcc0.pins.wave_out_pin_mux[4]    = MUX_PA14F_TCC0_WO4;
    config_tcc0.pins.enable_wave_out_pin[5] = true;
    config_tcc0.pins.wave_out_pin[5]        = PIN_PA15F_TCC0_WO5;
    config_tcc0.pins.wave_out_pin_mux[5]    = MUX_PA15F_TCC0_WO5;
    tcc_init(&tcc0_instance, TCC0, &config_tcc0);
    tcc_enable(&tcc0_instance);

    struct tcc_config config_tcc2;
    tcc_get_config_defaults(&config_tcc2, TCC2);
    config_tcc2.counter.period = 255;
    config_tcc2.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
    config_tcc2.compare.match[0] = 0;   /* meter duty, WO[0] */
    config_tcc2.pins.enable_wave_out_pin[0] = true;
    config_tcc2.pins.wave_out_pin[0]        = PIN_PA16E_TCC2_WO0;
    config_tcc2.pins.wave_out_pin_mux[0]    = MUX_PA16E_TCC2_WO0;
    tcc_init(&tcc2_instance, TCC2, &config_tcc2);
    tcc_enable(&tcc2_instance);
}

void set_lantern_indicator(uint8_t val) {
    tcc_set_compare_value(&tcc0_instance, TCC_MATCH_CAPTURE_CHANNEL_0, val); /* 0-255 */
}

void set_sword_indicator(uint8_t val) {
    tcc_set_compare_value(&tcc0_instance, TCC_MATCH_CAPTURE_CHANNEL_1, val);
}

void set_score_indicator(uint8_t val) {
    tcc_set_compare_value(&tcc2_instance, TCC_MATCH_CAPTURE_CHANNEL_0, val);
}

void open_lock(void) {
    
}

void handle_lock(void) {
    
}

uint8_t map_u8(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max)
{
    if (in_max == in_min)
        return out_min;

    return (uint8_t)(((uint16_t)(x - in_min) * (out_max - out_min)) /
                     (in_max - in_min) + out_min);
}
