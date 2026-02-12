BUILD_DIR := ./build
SRC_DIR := ./src
INO_NAME := $(notdir $(patsubst %/,%,$(SRC_DIR)))

MODEL := arduino:avr:pro
PORT := /dev/cu.usbserial-A700e5Q6

.PHONY: build upload clean

all: build upload

build:
	@touch $(SRC_DIR)/$(INO_NAME).ino
	arduino-cli compile \
  	--fqbn $(MODEL) \
  	--build-path $(BUILD_DIR) \
		$(SRC_DIR)
	@ln -sf $(BUILD_DIR)/compile_commands.json 
	@rm -rf $(SRC_DIR)/$(INO_NAME).ino
	@echo "-> Built $(SRC_DIR) to $(BUILD_DIR) for $(MODEL)"

upload:
	arduino-cli upload -p $(PORT) \
		--fqbn $(MODEL) \
		--build-path $(BUILD_DIR) \
		$(SRC_DIR)
	@echo "-> Uploaded $(BUILD_DIR) for $(MODEL) to $(PORT)"

clean:
	rm -rf $(BUILD_DIR) ./compile_commands.json 