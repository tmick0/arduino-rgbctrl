# arduino-rgbctrl

This is intended to become an Arduino program implementing a scriptable RGB controller that can be reprogrammed over a serial device.

## Overview

I wanted an RGB controller that would be easy to reprogram from any device or operating system. So, I implemented a virtual machine
on an Arduino and taught it to do stuff to LEDs.

The main goal of this project is to provide a more customizable LED controller than off-the-shelf ones, with modular support
for different types of LED chips.

Complete features:

- A virtual machine that interprets bytecode which manipulates the state of RGB values.
- EEPROM bytecode storage that the MCU reads back into memory at startup.
- Serial bytecode updater with persistence in EEPROM.
- An assembler that can translate mnemonics to bytecode.
- A simple PWM based driver for controlling an analog RGB LED.

TODO:

- Implement an actual addressable RGB LED driver (target platform TBD).
- Implement a GUI for easily programming and flashing the bytecode.

## Usage

The only driver currently implemented is `analog`, which runs RGB on pins 9, 10, and 11 respectively.

To produce output, an rgbvm program must:

1. Initialize a driver on a channel (`init` instruction)
2. Write as many pixels of data to that channel as desired (`write` instruction)
3. Instruct the driver to send the buffered data (`send` instruction)

Bytecode can be generated with [assemble.py](tools/assemble.py) and flashed to the Arduino with [flash.py](tools/flash.py).

If there is existing content in EEPROM, the firmware may malfunction due to attempting to interpret it as bytecode. If this happens, just write
a simple program to clear the first two bytes of the EEPROM and flash it before attempting to run this software again.

## Building

This project is based on [arduino-cmake](https://github.com/queezythegreat/arduino-cmake) and follows the standard CMake build process:

```
mkdir build
cd build
cmake ..
make
```

The framework conveniently provides  `make upload` to flash the Arduino firmware.

## Requirements

* Base requirements:

  - `CMake` - http://www.cmake.org/cmake/resources/software.html
  - `Arduino SDK` - http://www.arduino.cc/en/Main/Software

* Linux requirements:

  - `gcc-avr`      - AVR GNU GCC compiler
  - `binutils-avr` - AVR binary tools
  - `avr-libc`     - AVR C library
  - `avrdude`      - Firmware uploader

## Virtual machine

rgb-ctrl is based on an 8-bit virtual machine called rgbvm. It implements an application-specific instruction set, having 15 general purpose registers,
a flag register, an instruction pointer register, a few basic arithmetic operations, color space conversion operations, branching operations, and output controls.

Execution starts at instruction offset 0, resetting to 0 if the end of the program is reached.

### Opcodes

| Mnemonic | Operands           | Description                                                                    |
|----------|--------------------|--------------------------------------------------------------------------------|
| nop      | \[imm\]            | no operation -- if imm specified and nonzero, sleep 2^(imm-1) ms               |
| set      | rdst (rsrc\|imm)   | load register rdst from rsrc or immediate                                      |
| add      | rdst (rsrc\|imm)   | add to rdst from rsrc or immediate                                             |
| mul      | rdst (rsrc\|imm)   | multiply rdst by rsrc or immediate                                             |
| div      | rdst (rsrc\|imm)   | divide rdst by rsrc or immediate                                               |
| mod      | rdst (rsrc\|imm)   | modulo rdst by rsrc or immediate                                               |
| cmp      | r0 (r1\|imm)       | compare r0 to r1 or imm and store the result in flags                          |
| goto     | address            | move the instruction pointer to address                                        |
| brne     | address            | move the ip to address if last comparison was not equal                        |
| breq     | address            | move the ip to address if last comparison was equal                            |
| hsv2rgb  | rh rs rv           | convert rgb values in registers to hsv (inplace)                               |
| init     | immd immc \[immf\] | initialize output channel immc with driver number immd and optional flags immf |
| write    | rr rg rb immc      | buffer rgb value from registers on output channel immc                         |
| send     | immc               | activate the buffered output of immc                                           |

Take a look at [hue_cycle.rgbvm](scripts/hue_cycle.rgbvm) or [hue_cycle.rgbvm](scripts/value_pulse.rgbvm) for usage examples.

The assembler currently has a very basic syntax:

- A line starting with `#` is a comment
- A line starting with `:` is a label
- Every other non-empty line is expected to describe an instruction
- Each instruction is a sequence of space-separated tokens
- The first token is an opcode, and subsequent tokens are operands
- If an operand starts with `r`, it is a register; if it starts with a `:`, it is a label; otherwise, it is an immediate

The assembler runs two passes over the source: first, it determines the instruction addresses of all labels; then, it generates code.

The assembler depends on the package [bitstruct](https://pypi.org/project/bitstruct/).

## License

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
