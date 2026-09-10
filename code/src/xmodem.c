#include <stdint.h>
#include "xmodem.h"
#include "eeprom_slots.h"
#include "udi_cdc.h"            /* udi_cdc_is_rx_ready/getc/putc */
#include "system_time.h" /* get_uptime() -- 1 Hz uptime counter */

#define SOH   0x01
#define STX   0x02
#define EOT   0x04
#define ACK   0x06
#define NAK   0x15
#define CAN   0x18

/* Set to 0 if SRAM is tight -- drops buf[] from 1026 to 130 bytes and
 * NAKs any STX (1K) block header, forcing the sender to 128-byte
 * blocks (the default for most terminal "XMODEM" sends anyway --
 * 1K/XMODEM-1K is usually a separate option). */
//#define XMODEM_ALLOW_1K   1

#if XMODEM_ALLOW_1K
#define XMODEM_MAX_BLOCK  1024
#else
#define XMODEM_MAX_BLOCK  128
#endif

#define BLOCK_TIMEOUT_S    3   /* per-byte/per-field timeout within a block */
#define START_RETRY_S      3   /* how often to resend 'C' during handshake */
#define START_RETRIES      20  /* ~60s to give up waiting for a sender */
#define RETRY_LIMIT        10  /* NAKs on one block before giving up */

typedef enum {
    ST_START,   /* handshake: send 'C', wait for first header, resend periodically */
    ST_HEADER,  /* waiting for the header byte of a block (or EOT/CAN) */
    ST_BLKNUM,
    ST_BLKINV,
    ST_DATA
} state_t;

static state_t   state;
static uint16_t  write_offset;   /* how far into slot 1 we've written so far */
static uint8_t   buf[XMODEM_MAX_BLOCK + 2]; /* data + 2 CRC bytes */
static uint8_t   pending_header;
static uint16_t  data_len;
static uint16_t  data_idx;
static uint8_t   blk_num, blk_inv;
static uint8_t   expected_blk;
static uint32_t  state_enter_s;
static uint8_t   handshake_retries;
static uint8_t   block_retries;
static xmodem_status_t status;

static uint16_t crc16_ccitt(const uint8_t *p, uint16_t len)
{
    uint16_t crc = 0;
    while (len--) {
        crc ^= (uint16_t)(*p++) << 8;
        for (uint8_t i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
    }
    return crc;
}

static uint8_t elapsed(uint32_t since, uint8_t secs)
{
    /* unsigned subtraction wraps correctly even across get_uptime() rollover */
    return (uint8_t)((get_uptime() - since) >= secs);
}

static void nak_and_retry(void)
{
    while (udi_cdc_is_rx_ready()) udi_cdc_getc(); /* drop any trailing garbage */
    data_idx = 0;
    if (++block_retries >= RETRY_LIMIT) {
        status = XMODEM_DONE_ERROR;
        return;
    }
    udi_cdc_putc(NAK);
    state = ST_HEADER;
    state_enter_s = get_uptime();
}

static void finish_block(void)
{
    if ((uint8_t)(blk_num + blk_inv) != 0xFF) { nak_and_retry(); return; }
    uint16_t rx_crc = ((uint16_t)buf[data_len] << 8) | buf[data_len + 1];
    if (crc16_ccitt(buf, data_len) != rx_crc) { nak_and_retry(); return; }
 
    if (blk_num == expected_blk) {
        eeprom_status_t est = eeprom_slot_write(1, write_offset, buf, data_len);
        if (est == EEPROM_ERR_RANGE) {
            /* Slot 1 is full -- incoming file is bigger than SLOT_SIZE.
             * Abort cleanly rather than silently truncating or
             * spilling into whatever comes after slot 1. */
            udi_cdc_putc(CAN);
            udi_cdc_putc(CAN);
            status = XMODEM_DONE_ERROR;
            return;
        }
        if (est != EEPROM_OK) {
            /* bus error / write-cycle timeout -- fatal, same as the
             * original file-write-failure path did */
            udi_cdc_putc(CAN);
            udi_cdc_putc(CAN);
            status = XMODEM_DONE_ERROR;
            return;
        }
        write_offset += data_len;
        expected_blk++;
        udi_cdc_putc(ACK);
    } else if (blk_num == (uint8_t)(expected_blk - 1)) {
        udi_cdc_putc(ACK); /* dup -- our ACK for it was lost, don't rewrite */
    } else {
        udi_cdc_putc(CAN); /* out of sequence, give up */
        udi_cdc_putc(CAN);
        status = XMODEM_DONE_ERROR;
        return;
    }
 
    block_retries = 0;
    state = ST_HEADER;
    state_enter_s = get_uptime();
}

void xmodem_start(void)
{
    expected_blk = 1;
    handshake_retries = 0;
    block_retries = 0;
    data_idx = 0;
    write_offset = 0;
    status = XMODEM_BUSY;
    state = ST_START;
    state_enter_s = get_uptime() - START_RETRY_S;
    while (udi_cdc_is_rx_ready()) udi_cdc_getc(); /* flush stale input */
}

xmodem_status_t xmodem_poll(void)
{
    if (status != XMODEM_BUSY) return status;

    switch (state) {

    case ST_START:
        if (udi_cdc_is_rx_ready()) {
            uint8_t c = udi_cdc_getc();
            if (c == SOH || c == STX) {
                pending_header = c;
                data_idx = 0;
                state = ST_BLKNUM;
                state_enter_s = get_uptime();
            } else if (c == CAN) {
                status = XMODEM_DONE_ERROR;
            }
            /* anything else while handshaking: ignore, keep waiting */
        } else if (elapsed(state_enter_s, START_RETRY_S)) {
            if (++handshake_retries >= START_RETRIES) {
                status = XMODEM_DONE_ERROR;
            } else {
                udi_cdc_putc('C');
                state_enter_s = get_uptime();
            }
        }
        break;

    case ST_HEADER:
        if (udi_cdc_is_rx_ready()) {
            uint8_t c = udi_cdc_getc();
            if (c == EOT) {
                udi_cdc_putc(ACK);
                status = XMODEM_DONE_OK;
            } else if (c == CAN) {
                status = XMODEM_DONE_ERROR;
            } else if (c == SOH || c == STX) {
                pending_header = c;
                data_idx = 0;
                state = ST_BLKNUM;
                state_enter_s = get_uptime();
            } else {
                nak_and_retry(); /* garbage where a header was expected */
            }
        } else if (elapsed(state_enter_s, BLOCK_TIMEOUT_S)) {
            nak_and_retry();
        }
        break;

    case ST_BLKNUM:
        if (udi_cdc_is_rx_ready()) {
            blk_num = udi_cdc_getc();
            state = ST_BLKINV;
            state_enter_s = get_uptime();
        } else if (elapsed(state_enter_s, BLOCK_TIMEOUT_S)) {
            nak_and_retry();
        }
        break;

    case ST_BLKINV:
        if (udi_cdc_is_rx_ready()) {
            blk_inv = udi_cdc_getc();
#if XMODEM_ALLOW_1K
            data_len = (pending_header == STX) ? 1024 : 128;
#else
            data_len = 128;
            if (pending_header == STX) { nak_and_retry(); break; }
#endif
            state = ST_DATA;
            state_enter_s = get_uptime();
        } else if (elapsed(state_enter_s, BLOCK_TIMEOUT_S)) {
            nak_and_retry();
        }
        break;

    case ST_DATA:
        while (data_idx < (uint16_t)(data_len + 2) && udi_cdc_is_rx_ready()) {
            buf[data_idx++] = udi_cdc_getc();
            state_enter_s = get_uptime(); /* reset per-byte timeout */
        }
        if (data_idx == (uint16_t)(data_len + 2)) {
            finish_block();
        } else if (elapsed(state_enter_s, BLOCK_TIMEOUT_S)) {
            nak_and_retry();
        }
        break;
    }

    return status;
}
