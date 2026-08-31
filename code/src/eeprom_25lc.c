#include "eeprom_25lc.h"

static struct spi_module s_spi_instance;
static struct spi_module *s_spi = &s_spi_instance;

static inline void cs_low(void)  { port_pin_set_output_level(EEPROM_CS_PIN, false); }
static inline void cs_high(void) { port_pin_set_output_level(EEPROM_CS_PIN, true); }

void eeprom_hw_init(void)
{
    /* --- SPI peripheral (SERCOM4) --- */
    struct spi_config config_spi_master;
    spi_get_config_defaults(&config_spi_master);
    config_spi_master.mux_setting = EEPROM_SPI_MUX;
    config_spi_master.pinmux_pad0 = EEPROM_PINMUX_PAD0;
    config_spi_master.pinmux_pad1 = EEPROM_PINMUX_PAD1;
    config_spi_master.pinmux_pad2 = EEPROM_PINMUX_PAD2;
    config_spi_master.pinmux_pad3 = EEPROM_PINMUX_PAD3;
    /* 25LC256 supports up to 10MHz (Vcc>=2.5V) / 20MHz (Vcc>=4.5V) per
     * datasheet -- 4MHz is a conservative starting point; raise it
     * once basic reads/writes are confirmed working. */
    config_spi_master.mode_specific.master.baudrate = 4000000;

    spi_init(s_spi, EEPROM_SERCOM, &config_spi_master);
    spi_enable(s_spi);

    /* --- manual CS pin (PA08), idle high (active-low chip select) --- */
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);
    pin_conf.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(EEPROM_CS_PIN, &pin_conf);
    cs_high();
}

static eeprom_status_t send_write_enable(void)
{
    uint8_t cmd = EEPROM_CMD_WREN;
    cs_low();
    enum status_code st = spi_write_buffer_wait(s_spi, &cmd, 1);
    cs_high();
    return (st == STATUS_OK) ? EEPROM_OK : EEPROM_ERR_BUS;
}

static eeprom_status_t read_status_register(uint8_t *sr_out)
{
    uint8_t cmd = EEPROM_CMD_RDSR;
    cs_low();
    enum status_code st1 = spi_write_buffer_wait(s_spi, &cmd, 1);
    enum status_code st2 = spi_read_buffer_wait(s_spi, sr_out, 1, 0x00);
    cs_high();
    if (st1 != STATUS_OK || st2 != STATUS_OK) {
        return EEPROM_ERR_BUS;
    }
    return EEPROM_OK;
}

/* Poll the status register's WIP bit until the internal write cycle
 * (5ms typical per datasheet) completes, or give up after a timeout. */
static eeprom_status_t wait_write_complete(void)
{
    for (int attempt = 0; attempt < 50; attempt++) {
        uint8_t sr;
        eeprom_status_t st = read_status_register(&sr);
        if (st != EEPROM_OK) {
            return st;
        }
        if ((sr & EEPROM_SR_WIP) == 0) {
            return EEPROM_OK;
        }
        /* small spin/delay between polls -- replace with your platform's
         * delay_ms() if you'd rather not busy-wait here */
        for (volatile int d = 0; d < 2000; d++) { }
    }
    return EEPROM_ERR_TIMEOUT;
}

static eeprom_status_t write_one_page_chunk(uint16_t addr, const uint8_t *data, uint16_t len)
{
    eeprom_status_t st = send_write_enable();
    if (st != EEPROM_OK) {
        return st;
    }

    uint8_t header[3] = {
        EEPROM_CMD_WRITE,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
    };

    cs_low();
    enum status_code st1 = spi_write_buffer_wait(s_spi, header, sizeof(header));
    enum status_code st2 = STATUS_OK;
    if (st1 == STATUS_OK) {
        st2 = spi_write_buffer_wait(s_spi, (uint8_t *)data, len);
    }
    cs_high();

    if (st1 != STATUS_OK || st2 != STATUS_OK) {
        return EEPROM_ERR_BUS;
    }

    return wait_write_complete();
}

eeprom_status_t eeprom_write(uint16_t addr, const uint8_t *data, uint16_t len)
{
    if ((uint32_t)addr + len > EEPROM_TOTAL_SIZE) {
        return EEPROM_ERR_RANGE;
    }

    while (len > 0) {
        uint16_t page_offset = addr % EEPROM_PAGE_SIZE;
        uint16_t space_in_page = EEPROM_PAGE_SIZE - page_offset;
        uint16_t chunk = (len < space_in_page) ? len : space_in_page;

        eeprom_status_t st = write_one_page_chunk(addr, data, chunk);
        if (st != EEPROM_OK) {
            return st;
        }

        addr += chunk;
        data += chunk;
        len  -= chunk;
    }
    return EEPROM_OK;
}

eeprom_status_t eeprom_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    if ((uint32_t)addr + len > EEPROM_TOTAL_SIZE) {
        return EEPROM_ERR_RANGE;
    }

    uint8_t header[3] = {
        EEPROM_CMD_READ,
        (uint8_t)(addr >> 8),
        (uint8_t)(addr & 0xFF),
    };

    cs_low();
    enum status_code st1 = spi_write_buffer_wait(s_spi, header, sizeof(header));
    enum status_code st2 = STATUS_OK;
    if (st1 == STATUS_OK) {
        st2 = spi_read_buffer_wait(s_spi, data, len, 0x00);
    }
    cs_high();

    if (st1 != STATUS_OK || st2 != STATUS_OK) {
        return EEPROM_ERR_BUS;
    }
    return EEPROM_OK;
}
