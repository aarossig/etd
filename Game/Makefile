################################################################################
#
# Makefile for etd
#
################################################################################

# Configuration Options ########################################################

# Binary Name
BIN = etd

# Source Files
H_FILES = $(wildcard *.h)
C_FILES = $(wildcard *.c)
O_FILES = $(patsubst %.c, %.o, $(C_FILES))

# MCU Configuration
F_CPU = 16000000UL
MCU = atmega328p

# Compiler Configuration
OPT = O3
C_FLAGS = -Wall -mmcu=$(MCU) -$(OPT) -DF_CPU=$(F_CPU) -std=c99 -fshort-enums
CC = avr-gcc

# Programming
PORT = /dev/ttyACM0
PROGRAMMER = arduino

# Targets ######################################################################

all: $(BIN).hex

$(BIN).hex: $(BIN).elf
	avr-objcopy -O ihex $(BIN).elf $(BIN).hex

$(BIN).elf: $(O_FILES) $(H_FILES)
	$(CC) $(C_FLAGS) $(O_FILES) -o $(BIN).elf

%.o: %.c
	$(CC) $(C_FLAGS) -c $^ -o $@

program: all
	avrdude -p $(MCU) -P $(PORT) -c $(PROGRAMMER) -U flash:w:$(BIN).hex:i

clean:
	rm -f $(O_FILES)
	rm -f $(BIN).elf
	rm -f $(BIN).hex

