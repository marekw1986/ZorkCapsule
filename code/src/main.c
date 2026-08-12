#include <clock.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>
#include <compiler.h>
#include <parts.h>
#include <gclk.h>
#include <system.h>
#include <usb.h>
#include <usb_protocol_cdc.h>
#include <udi_cdc.h>
#include <udc.h>
#include <udd.h>
#include <usart.h>
#include "ztypes.h"
#include "system_time.h"
#include "conf_usb.h"
#include "main.h"

#define LED_PIN   17   // PA17 = D13
#define LED_GROUP 0    // Port group A

extern uint32_t _sstack, _estack;

uint32_t blink_timer;
struct usart_module usart_instance;

static void initialize_uart(void);

void stack_paint(void)
{
    uint32_t *p = &_sstack;
    while (p < &_estack) *p++ = 0xDEADBEEF;
}

uint32_t stack_high_water_mark(void)
{
    uint32_t *p = &_sstack;
    while (p < &_estack && *p == 0xDEADBEEF) p++;
    return (uint32_t)((uint8_t*)&_estack - (uint8_t*)p);
}

int main(void)
{
	system_init();   /* configures XOSC, DFLL, GCLKs per conf_clocks.h */
    stack_paint();

	/* Configure PA17 (D13) as output, drive high to sanity-check GPIO */
	PORT->Group[LED_GROUP].DIRSET.reg = (1 << LED_PIN);
	PORT->Group[LED_GROUP].OUTSET.reg = (1 << LED_PIN);	

	system_time_init();
	initialize_uart();
	
	udc_start();
    
    open_story();
    configure(V1, V8);
    initialize_screen();
    z_restart();
    
    printf("START!\r\n");

    while (1)
    {
        zork_handle();
        
        /*
        if (udi_cdc_is_rx_ready()) {
            char c = udi_cdc_getc();
            if (udi_cdc_is_tx_ready()) {
                udi_cdc_putc(c);
            }
        }
        */
        
        //printf("stack free: %lu\r\n", stack_high_water_mark());

        if ((uint32_t)(millis() - blink_timer) > 1000) {
            printf("TEST\r\n");
            blink_timer = millis();
            //PORT->Group[LED_GROUP].OUTTGL.reg = (1 << LED_PIN);
        }
        
        
    }

    return 0;
}

static void initialize_uart(void)
{
    struct usart_config config_usart;
    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    = 115200;
    config_usart.mux_setting = USART_RX_3_TX_2_XCK_3;
    config_usart.pinmux_pad0 = PINMUX_UNUSED;
    config_usart.pinmux_pad1 = PINMUX_UNUSED;
    config_usart.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2; /* TX = D1 */
    config_usart.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3; /* RX = D0 */

    while (usart_init(&usart_instance, SERCOM0, &config_usart) != STATUS_OK) {}

    usart_enable(&usart_instance);
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
    __asm volatile
    (
        " movs r0, #4       \n"
        " mov  r1, lr       \n"
        " tst  r0, r1       \n"
        " beq  _msp         \n"
        " mrs  r0, psp      \n"
        " b    _hfh         \n"
        "_msp:               \n"
        " mrs  r0, msp      \n"
        "_hfh:               \n"
        " ldr  r1, =hard_fault_handler_c \n"
        " bx   r1           \n"
    );
}

void hard_fault_handler_c(uint32_t *stack_frame)
{
    /* Exception stack frame layout: r0 r1 r2 r3 r12 lr pc xpsr */
    uint32_t r0  = stack_frame[0];
    uint32_t r1  = stack_frame[1];
    uint32_t r2  = stack_frame[2];
    uint32_t r3  = stack_frame[3];
    uint32_t r12 = stack_frame[4];
    uint32_t lr  = stack_frame[5];
    uint32_t pc  = stack_frame[6];
    uint32_t psr = stack_frame[7];

    printf("\r\n*** HARD FAULT ***\r\n");
    printf("PC  = 0x%08lX\r\n", pc);
    printf("LR  = 0x%08lX\r\n", lr);
    printf("R0  = 0x%08lX\r\n", r0);
    printf("R1  = 0x%08lX\r\n", r1);
    printf("R2  = 0x%08lX\r\n", r2);
    printf("R3  = 0x%08lX\r\n", r3);
    printf("R12 = 0x%08lX\r\n", r12);
    printf("PSR = 0x%08lX\r\n", psr);

    while (1) {
        PORT->Group[LED_GROUP].OUTTGL.reg = (1 << LED_PIN);
        for (volatile uint32_t i = 0; i < 200000; i++);
    }
}
