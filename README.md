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

rgb-ctrl is based on an 8-bit virtual machine called rgbvm. It implements an application-specific instruction set, having 15 general purpose registers,
a flag register, an instruction pointer register, a few basic arithmetic operations, color space conversion operations, branching operations, and an output operation.

Execution starts at instruction offset 0, resetting to 0 if the end of the program is reached.

### Opcodes

| Mnemonic | Operands         | Description                                                      |
|----------|------------------|------------------------------------------------------------------|
| nop      |                  | no operation                                                     |
| set      | rdst (rsrc\|imm) | load register rdst from rsrc or immediate                        |
| add      | rdst (rsrc\|imm) | add to rdst from rsrc or immediate                               |
| mul      | rdst (rsrc\|imm) | multiply rdst by rsrc or immediate                               |
| div      | rdst (rsrc\|imm) | divide rdst by rsrc or immediate                                 |
| mod      | rdst (rsrc\|imm) | modulo rdst by rsrc or immediate                                 |
| cmp      | rdst (rsrc\|imm) | compare rdst to imm and store the result in flags                |
| write    | rr rg rb oimm    | write rgb value from registers to output identified by immediate |
| hsv2rgb  | rh rs rv         | convert rgb values in registers to hsv (inplace)                 |
| goto     | address          | move the instruction pointer to address                          |
| brne     | address          | move the ip to address if last comparison was not equal          |
| breq     | address          | move the ip to address if last comparison was equal              |

Take a look at [hue_cycle.rgbvm](scripts/hue_cycle.rgbvm) or [hue_cycle.rgbvm](scripts/value_pulse.rgbvm) for usage examples.

The assembler currently has a very basic syntax:

- A line starting with `#` is a comment
- A line starting with `:` is a label
- Every other non-empty line is expected to describe an instruction
- Each instruction is a sequence of space-separated tokens
- The first token is an opcode, and subsequent tokens are operands
- If an operand starts with `r`, it is a register; if it starts with a `:`, it is a label; otherwise, it is an immediate

Note the assembler currently does only a single pass over the input, so branch instructions must refer to labels that
occur previously in the source. The virtual machine itself does support forward branching.

The assembler depends on the package [bitstruct](https://pypi.org/project/bitstruct/).

## License

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
