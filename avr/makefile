# Tools
CC 		 := avr-gcc
OBJCPY := avr-objcopy
UPLOAD := avrdude

# Dirs
BUILD_DIR   := ./build
OBJ_DIR			:= $(BUILD_DIR)/objs
SRC_DIR 	  := ./src
INCLUDE_DIR := $(SRC_DIR)/include/

# Settings
MMCU  						:= atmega328p
SDEV 							:= /dev/cu.usbserial-A700e5Q6
FLAGS 						:= -I$(INCLUDE_DIR) -I$(AVR_LIBC_INCLUDE) -Os -Wall -DF_CPU=16000000UL $(EXTRA_FLAGS)
EXTRA_CLANG_FLAGS := -D__AVR_ATmega328P__ -D__AVR_ATmega328__ -D__AVR_ARCH__=5

# Files
SRC         := $(shell find $(SRC_DIR) -type f -name '*.[csS]')
OBJ         := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRC))
TARGET_NAME := $(BUILD_DIR)/main
TARGET      := $(TARGET_NAME).elf

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/% 
	@mkdir -p $(dir $@)
	$(CC) -mmcu=$(MMCU) $(FLAGS) -c $< -o $@ 
	@jq -n \
		--arg dir "$(CURDIR)" \
		--arg file "$(abspath $<)" \
		--arg cmd "$(CC) -mmcu=$(MMCU) $(FLAGS) $(EXTRA_CLANG_FLAGS) -c $(abspath $<)" \
		'{directory: $$dir, file: $$file, command: $$cmd}' > $@.json

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) -mmcu=$(MMCU) $(FLAGS) $^ -o $@
	

compile_commands.json: $(OBJ)
	@jq -s '.' $(OBJ_DIR)/*.json > $@
	@echo "-> Generated compile_commands.json"

# Targets
.PHONY: pure build clean upload info dump

build: $(TARGET) compile_commands.json

pure: FLAGS += -nostartfiles -nostdlib -nodefaultlibs -g0 -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-ident
pure: build

upload: $(TARGET)
	$(OBJCPY) -O ihex $< $(TARGET_NAME).hex
	$(UPLOAD) -c arduino -p $(MMCU) -P $(SDEV) -b 57600 -U flash:w:$(TARGET_NAME).hex

info: $(TARGET)
	@avr-size --format=avr --mcu=atmega328p $<

dump: $(TARGET)
	@avr-objdump -D $< > $(TARGET_NAME).dump
	@echo "-> Generated obj-dump of $< in $(TARGET_NAME).dump"

clean: 
	rm -rf $(BUILD_DIR)