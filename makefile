BUILD_DIR   := ./build
SRC_DIR 	  := ./src
INCLUDE_DIR := $(SRC_DIR)/include/
INO_NAME 	  := $(notdir $(patsubst %/,%,$(SRC_DIR)))

FLAGS := -Wall -I$(INCLUDE_DIR)
MODEL := arduino:avr:pro
SDEV 	:= /dev/cu.usbserial-A700e5Q6

.PHONY: build upload clean

all: build upload

build:
	@touch $(SRC_DIR)/$(INO_NAME).ino
	arduino-cli compile \
  	--fqbn $(MODEL) \
  	--build-path $(BUILD_DIR) \
		$(foreach elem, $(FLAGS),--build-property build.extra_flags=\"$(subst __, ,$(elem))\") \
		$(SRC_DIR)
	@ln -sf $(BUILD_DIR)/compile_commands.json 
	@rm -rf $(SRC_DIR)/$(INO_NAME).ino
	@echo "-> Built $(SRC_DIR) to $(BUILD_DIR) for $(MODEL)"

upload:
	arduino-cli upload -p $(SDEV) \
		--fqbn $(MODEL) \
		--build-path $(BUILD_DIR) \
		$(SRC_DIR)
	@echo "-> Uploaded $(BUILD_DIR) for $(MODEL) to $(SDEV)"

clean:
	rm -rf $(BUILD_DIR) ./compile_commands.json 