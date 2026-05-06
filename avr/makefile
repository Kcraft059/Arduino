# Tools
CC 		 := avr-gcc
OBJCPY := avr-objcopy
UPLOAD := avrdude

# Dirs
BUILD_DIR   := ./build
OBJ_DIR			:= $(BUILD_DIR)/objs
SRC_DIR 	  := ./src
INCLUDE_DIR := $(SRC_DIR)/include/

# Flags
FLAGS := $(EXTRA_FLAGS) -Wall -I$(INCLUDE_DIR) -I$(AVR_LIBC_INCLUDE) 
MMCU := atmega328p
SDEV 	:= /dev/cu.usbserial-A700e5Q6

# Files
SRC := $(shell find $(SRC_DIR) -type f -name '*.[csS]')
OBJ := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRC))
TARGET_NAME := $(BUILD_DIR)/main
TARGET := $(TARGET_NAME).hex

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/% 
	@mkdir -p $(dir $@)
	$(CC) -mmcu=$(MMCU) $(FLAGS) -c $< -o $@ 

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) -mmcu=$(MMCU) $(FLAGS) $^ -o $(TARGET_NAME).elf
	$(OBJCPY) -O ihex $(TARGET_NAME).elf $@

# Targets
.PHONY: pure build clean upload

upload: $(TARGET)
	$(UPLOAD) -c arduino -p $(MMCU) -P $(SDEV) -b 57600 -U flash:w:$(TARGET)

build: $(TARGET)

pure: FLAGS += -nostartfiles
pure: build

clean: 
	rm -rf $(BUILD_DIR)