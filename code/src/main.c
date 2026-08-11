#include <clock.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>
#include <compiler.h>
#include <gclk.h>
#include <system.h>
#include <usb.h>
#include <usb_protocol_cdc.h>
#include <udi_cdc.h>
#include <udc.h>
#include <udd.h>
#include "ztypes.h"
#include "system_time.h"
#include "conf_usb.h"
#include "main.h"

#define LED_PIN   17   // PA17 = D13
#define LED_GROUP 0    // Port group A

uint32_t blink_timer;

int main(void)
{
	system_init();   /* configures XOSC, DFLL, GCLKs per conf_clocks.h */

	/* Configure PA17 (D13) as output, drive high to sanity-check GPIO */
	PORT->Group[LED_GROUP].DIRSET.reg = (1 << LED_PIN);
	PORT->Group[LED_GROUP].OUTSET.reg = (1 << LED_PIN);	

	system_time_init();
	
	udc_start();
    
    open_story();
    configure(V1, V8);
    initialize_screen();
    z_restart();

    while (1)
    {
        zork_handle();
        
        
        if (udi_cdc_is_rx_ready()) {
            char c = udi_cdc_getc();
            if (udi_cdc_is_tx_ready()) {
                udi_cdc_putc(c);
            }
        }
        

        if ((uint32_t)(millis() - blink_timer) > 1000) {
            blink_timer = millis();
            PORT->Group[LED_GROUP].OUTTGL.reg = (1 << LED_PIN);
        }  
        
    }

    return 0;
}

void main_suspend_action(void) {

}

void main_resume_action(void) {

}

void main_sof_action(void) {

}

bool main_cdc_enable(uint8_t port)
{
	//main_b_cdc_enable = true;
	//configure_i2c_master();
	return true;
}

void main_cdc_disable(uint8_t port)
{
	//main_b_cdc_enable = false;
	//b_com_port_opened = false;
	//i2c_master_disable(&i2c_master_instance);
}
void cdc_rx_notify(uint8_t port)
{
	//b_cdc_data_rx = true;

}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
	/*
	if (b_enable) {
		ui_com_open(port);
		b_com_port_opened = true;
	}else{
		ui_com_close(port);
		b_com_port_opened = false;
	}
	*/
}

void HardFault_Handler(void)
{
    while (1) {
        PORT->Group[LED_GROUP].OUTTGL.reg = (1 << LED_PIN);
        for (volatile uint32_t i = 0; i < 200000; i++);  // fast blink = fault
    }
}
