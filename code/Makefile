CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

OPENOCD = openocd
OPENOCD_INTERFACE = interface/stlink.cfg
OPENOCD_TRANSPORT = hla_swd
OPENOCD_TARGET = target/at91samdXX.cfg

ASF = /opt/microchip/xdk-asf-3.50.0

INCLUDES = \
	-Iinclude \
	-Izork \
	-isystem $(ASF)/sam0/utils \
	-isystem $(ASF)/sam0/utils/preprocessor \
	-isystem $(ASF)/sam0/utils/header_files \
	-isystem $(ASF)/sam0/utils/cmsis/samd21/include \
	-isystem $(ASF)/thirdparty/CMSIS/Include \
	-isystem $(ASF)/common/boards \
	-isystem $(ASF)/common/utils \
	-isystem $(ASF)/common/services/sleepmgr \
	-isystem $(ASF)/common/services/usb \
	-isystem $(ASF)/common/services/usb/udc \
	-isystem $(ASF)/common/services/usb/class/cdc \
	-isystem $(ASF)/common/services/usb/class/cdc/device \
	-isystem $(ASF)/sam0/drivers/extint \
	-isystem $(ASF)/sam0/drivers/gpio \
	-isystem $(ASF)/sam0/drivers/port \
	-isystem $(ASF)/sam0/drivers/system \
	-isystem $(ASF)/sam0/drivers/system/clock \
	-isystem $(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1 \
	-isystem $(ASF)/sam0/drivers/system/interrupt \
	-isystem $(ASF)/sam0/drivers/system/interrupt/system_interrupt_samd21 \
	-isystem $(ASF)/sam0/drivers/system/pinmux \
	-isystem $(ASF)/sam0/drivers/system/power/power_sam_d_r_h \
	-isystem $(ASF)/sam0/drivers/system/reset/reset_sam_d_r_h \
	-isystem $(ASF)/sam0/drivers/sercom \
	-isystem $(ASF)/sam0/drivers/sercom/usart \
	-isystem $(ASF)/sam0/drivers/usb \
	-isystem $(ASF)/sam0/drivers/usb/usb_sam_d_r \
	-isystem $(ASF)/sam0/drivers/usb/stack_interface \
	-isystem $(ASF)/sam0/utils/cmsis/samd21/source \

CFLAGS = \
    -mcpu=cortex-m0plus \
    -mthumb \
    -D__SAMD21E18A__ \
    -Os \
    -g \
    -Wall \
    -ffunction-sections \
    -fdata-sections \
    $(INCLUDES) \

LDFLAGS = \
    -mcpu=cortex-m0plus \
    -mthumb \
    --specs=nosys.specs \
    -Wl,--defsym,__stack_size__=0x800 \
    -Wl,--defsym,__heap_size__=0x400 \
    -Wl,-T,samd21e18a_flash.ld \
    -Wl,--gc-sections \
	-Wl,-Map,$(TARGET).map,--cref
	
LIBS = \
    

BUILD = build
TARGET = $(BUILD)/zorkcapsule

CSRCS = \
	src/main.c \
	src/system_time.c \
	src/syscalls.c \
	zork/configur.c \
	zork/control.c \
	zork/extern.c \
	zork/fileio.c \
	zork/input.c \
	zork/interpre.c \
	zork/math.c \
	zork/object.c \
	zork/operand.c \
	zork/osdepend.c \
	zork/property.c \
	zork/screen.c \
	zork/text.c \
	zork/variable.c \
	$(ASF)/common/services/sleepmgr/samd/sleepmgr.c \
	$(ASF)/sam0/drivers/extint/extint_sam_d_r_h/extint.c \
	$(ASF)/sam0/drivers/port/port.c \
	$(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c \
	$(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c \
	$(ASF)/sam0/drivers/system/system.c \
	$(ASF)/sam0/drivers/system/interrupt/system_interrupt.c \
	$(ASF)/sam0/drivers/system/pinmux/pinmux.c \
	$(ASF)/sam0/utils/cmsis/samd21/source/system_samd21.c \
	$(ASF)/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c \
	$(ASF)/common/utils/interrupt/interrupt_sam_nvic.c \
	$(ASF)/sam0/drivers/usb/usb_sam_d_r/usb.c \
	$(ASF)/sam0/drivers/usb/stack_interface/usb_device_udd.c \
	$(ASF)/sam0/drivers/sercom/sercom.c \
	$(ASF)/sam0/drivers/sercom/sercom_interrupt.c \
	$(ASF)/sam0/drivers/sercom/usart/usart.c \
	$(ASF)/common/services/usb/udc/udc.c \
	$(ASF)/common/services/usb/class/cdc/device/udi_cdc.c \
	$(ASF)/common/services/usb/class/cdc/device/udi_cdc_desc.c
	
ASRCS = \

OBJS = $(CSRCS:%.c=$(BUILD)/%.o) $(patsubst /%.S,$(BUILD)/%.o,$(ASRCS))

all: $(TARGET).elf $(TARGET).bin $(TARGET).hex

$(TARGET).elf: $(OBJS)
	mkdir -p $(BUILD)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(SIZE) $@

$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
	
$(BUILD)/%.o: /%.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	
flash: $(TARGET).elf
	$(OPENOCD) \
		-f $(OPENOCD_INTERFACE) \
		-c "transport select $(OPENOCD_TRANSPORT)" \
		-c "set CPUTAPID 0x0bc11477" \
		-f $(OPENOCD_TARGET) \
		-c "adapter speed 1000" \
		-c "program $< verify reset exit"

clean:
	rm -rf $(BUILD)

.PHONY: all clean
