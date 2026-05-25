# Tools
CC 		 := avr-gcc
OBJCPY := avr-objcopy
UPLOAD := avrdude

# Locations
BUILD_DIR   := ./build
OBJ_DIR			:= $(BUILD_DIR)/objs
SRC_DIR 	  := ./src
INCLUDE_DIR := $(SRC_DIR)/include/
SDEV 				:= /dev/cu.usbserial-A700e5Q6

# Settings
MMCU  						:= atmega328p
ifeq ($(MMCU), atmega328p)
		ARCH_FLAGS 	:= -DF_CPU=16000000UL
		ARCH_CFLAGS := -D__AVR_ATmega328P__ -D__AVR_ATmega328__ -D__AVR_ARCH__=5
    ARCH_LFLAGS := --defsym __VECTOR_TABLE_START=0x0 --defsym __VECTOR_TABLE_END=0x68
endif
FLAGS 			:= -mmcu=$(MMCU) -I$(INCLUDE_DIR) -I$(AVR_LIBC_INCLUDE) -Os -Wall $(ARCH_FLAGS) $(EXTRA_FLAGS) 
LFLAGS 			:= $(ARCH_LFLAGS) $(EXTRA_LFLAGS)
CLANG_FLAGS := $(ARCH_CFLAGS)

# Files
SRC         := $(shell find $(SRC_DIR) -type f -name '*.[csS]')
OBJ         := $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRC))
TARGET_NAME := $(BUILD_DIR)/main
TARGET      := $(TARGET_NAME).elf
LD_SCRIPT 	:= ./avr.ld

,:=,

# Compile
$(OBJ_DIR)/%.o: $(SRC_DIR)/% 
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@ 
	@jq -n \
		--arg dir "$(CURDIR)" \
		--arg file "$(abspath $<)" \
		--arg cmd "$(CC) -mmcu=$(MMCU) $(FLAGS) $(CLANG_FLAGS) -c $(abspath $<)" \
		'{directory: $$dir, file: $$file, command: $$cmd}' > $@.json

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) $(addprefix -Wl$(,),$(LFLAGS)) $^ -o $@
	

compile_commands.json: $(OBJ)
	@jq -s '.' $(OBJ_DIR)/*.json > $@
	@echo "-> Generated compile_commands.json"

.asm-lsp.toml:
	@printf '[default_config]\n' > $@
	@printf 'version = "0.10.1"\n' >> $@
	@printf 'assembler = "avr"\n' >> $@
	@printf 'instruction_set = "AVR"\n' >> $@
	@printf '\n' >> $@
	@printf '[default_config.opts]\n' >> $@
	@printf 'compiler = "avr-gcc"\n' >> $@
	@printf 'compile_flags_txt = [%s]\n' '$(foreach elem,$(FLAGS) -o /dev/null,"$(elem)"$(,))' >> $@
	@printf 'diagnostics = true\n' >> $@
	@printf 'default_diagnostics = false\n' >> $@
	@echo "-> Generated .asm-lsp.toml"

# Targets
.PHONY: pure build clean upload info dump .asm-lsp.toml

build: $(TARGET) compile_commands.json .asm-lsp.toml

pure: $(LD_SCRIPT)
pure: FLAGS += -ffunction-sections -fdata-sections -nostartfiles -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-ident
# -nostdlib -nodefaultlibs
pure: LFLAGS += -T $(LD_SCRIPT) --gc-sections --strip-debug
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
