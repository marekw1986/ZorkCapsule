#ifndef XMODEM_H
#define XMODEM_H

#include <stdint.h>

typedef enum {
    XMODEM_BUSY = 0,
    XMODEM_DONE_OK,
    XMODEM_DONE_ERROR
} xmodem_status_t;

/* Starts a non-blocking XMODEM-CRC/1K receive into fp (already open
 * for writing). Does not transmit anything itself -- the first
 * protocol byte goes out on the next xmodem_poll() call, so any text
 * your command handler prints after calling this is safely out the
 * door before the wire protocol starts. */
void xmodem_start(void);

/* Call once per main-loop iteration while a transfer is in progress.
 * Non-blocking: acts on whatever's already sitting in the CDC RX
 * buffer and returns immediately, driven by get_uptime() for
 * timeouts. Returns XMODEM_BUSY until the transfer finishes,
 * fails, or is cancelled by the sender. */
xmodem_status_t xmodem_poll(void);

#endif
