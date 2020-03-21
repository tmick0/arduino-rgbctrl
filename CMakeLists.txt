
set(CMAKE_TOOLCHAIN_FILE cmake/ArduinoToolchain.cmake) # Arduino Toolchain
set(ARDUINO_DEFAULT_BOARD uno)
set(ARDUINO_DEFAULT_PORT /dev/ttyACM0)

cmake_minimum_required(VERSION 2.8)
project(arduino-rgbctrl C CXX)
add_subdirectory(src)
