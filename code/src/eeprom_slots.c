#include "eeprom_slots.h"

eeprom_status_t eeprom_slot_write(uint8_t slot, uint16_t offset, const uint8_t *data, uint16_t len)
{
    if (slot >= SLOT_COUNT) {
        return EEPROM_ERR_RANGE;
    }
    if ((uint32_t)offset + len > SLOT_SIZE) {
        return EEPROM_ERR_RANGE;   /* refuse -- would spill into the next slot */
    }
    uint16_t base = (uint16_t)(slot * SLOT_SIZE);
    return eeprom_write((uint16_t)(base + offset), data, len);
}

eeprom_status_t eeprom_slot0_write(uint16_t offset, const uint8_t *data, uint16_t len)
{
    /* kept for XMODEM, which is still restricted to slot 0 only */
    return eeprom_slot_write(0, offset, data, len);
}

eeprom_status_t eeprom_slot_read(uint8_t slot, uint16_t offset, uint8_t *data, uint16_t len)
{
    if (slot >= SLOT_COUNT) {
        return EEPROM_ERR_RANGE;
    }
    if ((uint32_t)offset + len > SLOT_SIZE) {
        return EEPROM_ERR_RANGE;
    }
    uint16_t base = (uint16_t)(slot * SLOT_SIZE);
    return eeprom_read((uint16_t)(base + offset), data, len);
}
