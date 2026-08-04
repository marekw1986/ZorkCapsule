#ifndef _MAIN_H_
#define _MAIN_H_

void main_suspend_action(void);
void main_resume_action(void);
void main_sof_action(void);
bool main_cdc_enable(uint8_t port);
void main_cdc_disable(uint8_t port);
void cdc_rx_notify(uint8_t port);
void main_cdc_set_dtr(uint8_t port, bool b_enable);

#endif // _MAIN_H
