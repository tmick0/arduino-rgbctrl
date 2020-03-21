# arduino-rgbctrl

This is intended to become an Arduino program implementing a scriptable RGB controller that can be reprogrammed over a serial device.

Goals:

- Implement a "virtual machine" that interprets bytecode corresponding to commands which manipulate the state of addressable RGB LEDs.
- Implement an actual RGB LED driver (target platform TBD).
- Store bytecode in EEPROM and have the MCU read it back into memory at startup.
- Allow bytecode updates over the serial interface to be stored in EEPROM.
- Implement an assembler that can translate mnemonics to bytecode.
- Implement a GUI for easily programming and flashing the bytecode.

## Building

This project is based on [arduino-cmake](https://github.com/queezythegreat/arduino-cmake) and follows the standard CMake build process:

```
mkdir build
cd build
cmake ..
make
```

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

rgb-ctrl is based on a virtual machine called rgbvm. It implements an application-specific instruction set, having 15 registers, a few basic arithmetic operations, color space conversion operations, and an output operation.

Currently, rgbvm does not support branching and runs a loop over the bytecode starting from instruction offset 0, resetting to 0 when reaching the end of the program.

### Opcodes

| Mnemonic | Operands         | Description                                                      |
|----------|------------------|------------------------------------------------------------------|
| nop      |                  | no operation                                                     |
| set      | rdst (rsrc\|imm) | load register rdst from rsrc or immediate                        |
| add      | rdst (rsrc\|imm) | add to rdst from rsrc or immediate                               |
| mul      | rdst (rsrc\|imm) | multiply rdst by rsrc or immediate                               |
| div      | rdst (rsrc\|imm) | divide rdst by rsrc or immediate                                 |
| mod      | rdst (rsrc\|imm) | modulo rdst by rsrc or immediate                                 |
| write    | rr rg rb oimm    | write rgb value from registers to output identified by immediate |
| hsv2rgb  | rh rs rv         | convert rgb values in registers to hsv (inplace)                 |

Take a look at [scripts/hue_cycle.rgbvm](hue_cycle.rgbvm) for a usage example.

The assembler currently has a very basic syntax:

- A line starting with `#` is a comment
- Every other non-empty line is expected to describe an instruction
- Each instruction is a sequence of space-separated tokens
- The first token is an opcode, and subsequent tokens are operands
- If an operand starts with `r`, it is a register; otherwise, it is an immediate

The assembler depends on the package [bitstruct](https://pypi.org/project/bitstruct/).

## License

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
