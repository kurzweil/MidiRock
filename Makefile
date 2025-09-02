# Target MCU and architecture
MCU          = atmega32u4
ARCH         = AVR8
F_CPU        = 16000000UL
F_USB        = $(F_CPU)

# Paths
LUFA_PATH    = vendor/lufa/LUFA
DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA

# Program name
TARGET       = rockband

# Compiler and tools
CC           = avr-gcc
OBJ2HEX      = avr-objcopy
PROGRAMMER   = avrispmkII
PORT         = usb  # tells avrdude to talk directly to the programmer

# Compiler flags
CFLAGS       = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DF_USB=$(F_USB) -DUSE_LUFA_CONFIG_HEADER -IConfig/ -Ivendor/lufa -I$(LUFA_PATH)/Drivers -Os

# Source files
SRC          = $(TARGET).c Descriptors.c \
	$(LUFA_ROOT_PATH)/Drivers/USB/Core/$(ARCH)/USBController_$(ARCH).c   \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/$(ARCH)/USBInterrupt_$(ARCH).c    \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/ConfigDescriptors.c               \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/Events.c                          \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/USBTask.c                         \
	$(LUFA_ROOT_PATH)/Drivers/USB/Core/$(ARCH)/Device_$(ARCH).c          \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/$(ARCH)/Endpoint_$(ARCH).c        \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/$(ARCH)/EndpointStream_$(ARCH).c  \
        $(LUFA_ROOT_PATH)/Drivers/USB/Core/DeviceStandardReq.c

include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

# Object files
OBJ          = $(SRC:.c=.o)

# Default target
all: $(TARGET).hex

# Compile C files into objects
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link objects into ELF
$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

# Convert ELF to HEX for flashing
$(TARGET).hex: $(TARGET).elf
	$(OBJ2HEX) -O ihex -R .eeprom $< $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

# Flash the device
flash: $(TARGET).hex
	avrdude -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$<:i

