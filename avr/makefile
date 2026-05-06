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
TARGET      := $(TARGET_NAME).hex

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
	$(CC) -mmcu=$(MMCU) $(FLAGS) $^ -o $(TARGET_NAME).elf
	$(OBJCPY) -O ihex $(TARGET_NAME).elf $@

compile_commands.json: $(OBJ)
	@jq -s '.' $(OBJ_DIR)/*.json > $@
	@echo "-> Generated compile_commands.json"

# Targets
.PHONY: pure build clean upload

upload: $(TARGET)
	$(UPLOAD) -c arduino -p $(MMCU) -P $(SDEV) -b 57600 -U flash:w:$(TARGET)

build: $(TARGET) compile_commands.json

pure: FLAGS += -nostartfiles
pure: build

clean: 
	rm -rf $(BUILD_DIR)