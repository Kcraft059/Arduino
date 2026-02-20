# Arduino 

## Arduino-cli

> [!NOTE]
> Default arduino packages path: `~/Library/Arduino*` redirected to `./arduino-data`

Create new sketch
```bash
arduino-cli sketch new <sketch_path>
```
Update core drivers & compilers
```bash
arduino-cli core update-index
```
Install drivers and compilers for board
```bash
# Search
arduino-cli core search <name>
# Install
arduino-cli core install <core:id>
# List installed 
arduino-cli core list
```

Search for board FQBN (eg : `arduino:avr:pro`)
```bash
arduino-cli board listall <board_name>
```
Compile sketch for board
```bash
arduino-cli compile \
  --fqbn <fqbn> \
  --build-path <build_dir> \
  ./sketch
```
Find serial device to board (eg: `/dev/cu.usbserial-A700e5Q6`)
```bash
arduino-cli board list
```
Upload to board
```bash
arduino-cli upload -p <sdev> \
  --fqbn <fqbn> \
  --build-path <build_dir> \
  <sketch_path>
```

## Serial console

Open using arduino-cli 
```bash
arduino-cli monitor -p <sdev> -c <baud>
```

Open in raw mode with screen
```bash
screen <sdev> <baud>
```

Or with cat, line buffered
```bash
stty -F <sdev> <baud>
cat <sdev> &
echo <mycommand> > <sdev>
```