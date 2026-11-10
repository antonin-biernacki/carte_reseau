# Compilateur et outils
CC = avr-gcc
OBJCOPY = avr-objcopy
DFU = dfu-programmer

# Options de compilation
CFLAGS = -mmcu=atmega32u4 -Wall -Werror -I. -DF_CPU=16000000 -Os
TARGET = test
SRC = test.c

# Règles
all: $(TARGET).hex

%.elf: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

flash: $(TARGET).hex
	$(DFU) atmega32u4 erase
	$(DFU) atmega32u4 flash $(TARGET).hex
	$(DFU) atmega32u4 reset

clean:
	rm -f $(TARGET).elf $(TARGET).hex

.PHONY: all flash clean

