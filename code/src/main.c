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

int main(void)
{
	system_init();   /* configures XOSC, DFLL, GCLKs per conf_clocks.h */
	system_time_init();
	
	//udc_start();

    while (1)
    {
        zork_handle();
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
