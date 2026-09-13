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
    config_tcc0.compare.match[2] = 0;   /* lantern duty, CC2 -> WO[2] */
    config_tcc0.compare.match[3] = 0;   /* sword duty, CC3 -> WO[3] */
    config_tcc0.pins.enable_wave_out_pin[2] = true;
    config_tcc0.pins.wave_out_pin[2]        = PIN_PA18F_TCC0_WO2;
    config_tcc0.pins.wave_out_pin_mux[2]    = MUX_PA18F_TCC0_WO2;
    config_tcc0.pins.enable_wave_out_pin[3] = true;
    config_tcc0.pins.wave_out_pin[3]        = PIN_PA19F_TCC0_WO3;
    config_tcc0.pins.wave_out_pin_mux[3]    = MUX_PA19F_TCC0_WO3;
    tcc_init(&tcc0_instance, TCC0, &config_tcc0);
    tcc_enable(&tcc0_instance);

    struct tcc_config config_tcc2;
    tcc_get_config_defaults(&config_tcc2, TCC2);
    config_tcc2.counter.period = 255;
    config_tcc2.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
    config_tcc2.compare.match[0] = 0;
    config_tcc2.pins.enable_wave_out_pin[0] = true;
    config_tcc2.pins.wave_out_pin[0]        = PIN_PA16E_TCC2_WO0;
    config_tcc2.pins.wave_out_pin_mux[0]    = MUX_PA16E_TCC2_WO0;
    tcc_init(&tcc2_instance, TCC2, &config_tcc2);
    tcc_enable(&tcc2_instance);
}

void set_lantern_indicator(uint8_t val) {
    tcc_set_compare_value(&tcc0_instance, TCC_MATCH_CAPTURE_CHANNEL_2, val);
}

void set_sword_indicator(uint8_t val) {
    tcc_set_compare_value(&tcc0_instance, TCC_MATCH_CAPTURE_CHANNEL_3, val);
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

uint16_t map_u16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    if (in_max == in_min)
        return out_min;
    return (uint16_t)(((uint32_t)(x - in_min) * (out_max - out_min)) /
                      (in_max - in_min) + out_min);
}
