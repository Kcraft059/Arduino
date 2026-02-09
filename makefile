BUILD_DIR := ./build
SRC_DIR := ./src
INO_NAME := $(notdir $(patsubst %/,%,$(SRC_DIR)))

MODEL := arduino:avr:pro
PORT := /dev/cu.usbserial-A700e5Q6

.PHONY: build upload clean

build:
	@touch $(SRC_DIR)/$(INO_NAME).ino
	arduino-cli compile \
  	--fqbn $(MODEL) \
  	--build-path $(BUILD_DIR) \
		$(SRC_DIR)
	@rm -rf $(SRC_DIR)/$(INO_NAME).ino

upload:
	arduino-cli upload -p $(PORT) \
		--fqbn $(MODEL) \
		--build-path $(BUILD_DIR) \
		$(SRC_DIR)

clean:
	rm -rf $(BUILD_DIR)