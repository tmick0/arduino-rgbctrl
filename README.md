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


## License

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
