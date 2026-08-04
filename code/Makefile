CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

ASF = /opt/microchip/xdk-asf-3.50.0

INCLUDES = \
	-Iinclude \
	-Izork \
	-I$(ASF)/sam0/utils \
	-I$(ASF)/sam0/utils/preprocessor \
	-I$(ASF)/sam0/utils/header_files \
	-I$(ASF)/sam0/utils/cmsis/samd21/include \
	-I$(ASF)/thirdparty/CMSIS/Include \
	-I$(ASF)/common/utils \
	-I$(ASF)/sam0/drivers/system \
	-I$(ASF)/sam0/drivers/system/clock \
	-I$(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1 \
	-I$(ASF)/sam0/drivers/system/interrupt \
	-I$(ASF)/sam0/drivers/system/interrupt/system_interrupt_samd21 \
	-I$(ASF)/sam0/drivers/system/pinmux \
	-I$(ASF)/sam0/drivers/system/power/power_sam_d_r_h \
	-I$(ASF)/sam0/drivers/system/reset/reset_sam_d_r_h \
	-I$(ASF)/sam0/utils/cmsis/samd21/source \

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
	$(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/clock.c \
	$(ASF)/sam0/drivers/system/clock/clock_samd21_r21_da_ha1/gclk.c \
	$(ASF)/sam0/drivers/system/system.c \
	$(ASF)/sam0/drivers/system/interrupt/system_interrupt.c \
	$(ASF)/sam0/drivers/system/pinmux/pinmux.c \
	$(ASF)/sam0/utils/cmsis/samd21/source/system_samd21.c \
	$(ASF)/sam0/utils/cmsis/samd21/source/gcc/startup_samd21.c \
	$(ASF)/common/utils/interrupt/interrupt_sam_nvic.c
	
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

clean:
	rm -rf $(BUILD)

.PHONY: all clean
