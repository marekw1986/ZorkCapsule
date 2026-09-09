/*
 * eeprom_slots.h
 *
 * Divides the EEPROM into 2 fixed 16384-byte slots (exact fit for the
 * 25LC256's 32768-byte total capacity). Sized to hold a full Zork I
 * save (dynamic memory is ~11859 bytes per fileio.c's
 * MAX_DYNAMIC_MEMORY), with ~4.5KB of headroom per slot for a
 * magic/length/checksum header.
 *
 * Per your spec: XMODEM only ever writes into slot 0, and writing is
 * refused once it would run past the end of that slot. z_save/z_restore
 * also use slot 0 (single in-game save slot) for the same reason.
 *
 * Requires EEPROM_TOTAL_SIZE >= SLOT_SIZE * SLOT_COUNT (checked at
 * compile time below). Works unmodified against either eeprom_24lc.h
 * (I2C, 24LC128) or eeprom_25lc.h (SPI, 25LC256) -- both expose the
 * same eeprom_status_t / eeprom_write / eeprom_read interface, so
 * only the #include below needs to change.
 */

#ifndef EEPROM_SLOTS_H
#define EEPROM_SLOTS_H

#include <stdint.h>
#include "eeprom_25lc.h"   /* swap for "eeprom_24lc.h" to use the I2C driver instead */

#define SLOT_SIZE    16384u
#define SLOT_COUNT   2u

_Static_assert(EEPROM_TOTAL_SIZE >= (uint32_t)SLOT_SIZE * SLOT_COUNT,
               "EEPROM too small for 2 x 16384-byte slots -- check EEPROM_TOTAL_SIZE in eeprom_25lc.h");

/* Write `len` bytes at `offset` within `slot` (0 or 1). */
eeprom_status_t eeprom_slot_write(uint8_t slot, uint16_t offset, const uint8_t *data, uint16_t len);

/* Write `len` bytes at `offset` within slot 0 only -- kept for XMODEM,
 * which is still restricted to slot 0 regardless of how many game
 * save slots exist. Thin wrapper around eeprom_slot_write(0, ...). */
eeprom_status_t eeprom_slot0_write(uint16_t offset, const uint8_t *data, uint16_t len);

eeprom_status_t eeprom_slot_read(uint8_t slot, uint16_t offset, uint8_t *data, uint16_t len);

#endif /* EEPROM_SLOTS_H */
