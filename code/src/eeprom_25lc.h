/*
 * eeprom_25lc.h
 *
 * Driver for the Microchip 25LC256 SPI EEPROM (32768 bytes, 64-byte
 * page) on ATSAMD21 via ASF3's spi_master (SERCOM SPI) driver.
 *
 * Same public interface (eeprom_status_t / eeprom_write / eeprom_read)
 * as eeprom_24lc.h, so eeprom_slots.c works unmodified against either
 * driver -- just swap which of the two headers it includes.
 */

#ifndef EEPROM_25LC_H
#define EEPROM_25LC_H

#include <stdint.h>
#include <stdbool.h>
#include <port.h>
#include <spi.h>

/* ---- Chip geometry ---- */
#define EEPROM_TOTAL_SIZE     32768u   /* 25LC256 = 32768 bytes */
#define EEPROM_PAGE_SIZE      64u

/* Chip-select pin -- set this to whatever pin you've wired CS to. */
#define EEPROM_CS_PIN         PIN_PA08

/* SERCOM instance and SPI signal routing for this board's wiring:
 *   MOSI = PB08 (SERCOM4 PAD0)
 *   SCK  = PB09 (SERCOM4 PAD1)
 *   MISO = PB11 (SERCOM4 PAD3)
 *   PAD2 (PB10, the hardware SS line) is left unused -- CS is done
 *   manually on PA08 instead, which is what EEPROM_CS_PIN is for. */
#define EEPROM_SERCOM         SERCOM4
#define EEPROM_SPI_MUX        SPI_SIGNAL_MUX_SETTING_D  /* DOPO=0 (DO=PAD0,SCK=PAD1), DIPO=3 (MISO=PAD3) */
#define EEPROM_PINMUX_PAD0    PINMUX_PB08D_SERCOM4_PAD0  /* MOSI */
#define EEPROM_PINMUX_PAD1    PINMUX_PB09D_SERCOM4_PAD1  /* SCK  */
#define EEPROM_PINMUX_PAD2    PINMUX_UNUSED              /* unused (manual CS instead) */
#define EEPROM_PINMUX_PAD3    PINMUX_PB11D_SERCOM4_PAD3  /* MISO */

/* 25LC256 instruction set */
#define EEPROM_CMD_READ   0x03u
#define EEPROM_CMD_WRITE  0x02u
#define EEPROM_CMD_WRDI   0x04u  /* write disable */
#define EEPROM_CMD_WREN   0x06u  /* write enable -- must precede every write */
#define EEPROM_CMD_RDSR   0x05u  /* read status register */
#define EEPROM_CMD_WRSR   0x01u  /* write status register */
#define EEPROM_SR_WIP     0x01u  /* write-in-progress bit in status register */

typedef enum {
    EEPROM_OK = 0,
    EEPROM_ERR_BUS,
    EEPROM_ERR_TIMEOUT,
    EEPROM_ERR_RANGE
} eeprom_status_t;

/* Call once at startup. Owns its own spi_module instance internally,
 * configures SERCOM4 per EEPROM_SERCOM/EEPROM_SPI_MUX/EEPROM_PINMUX_*
 * above, sets up the manual CS pin, and enables the peripheral --
 * nothing else needs to be done by the caller first. */
void eeprom_hw_init(void);

/* Page-boundary-safe write with WREN + status-register polling for
 * write-cycle completion. Blocking. */
eeprom_status_t eeprom_write(uint16_t addr, const uint8_t *data, uint16_t len);

/* Sequential read. Blocking. */
eeprom_status_t eeprom_read(uint16_t addr, uint8_t *data, uint16_t len);

#endif /* EEPROM_25LC_H */
