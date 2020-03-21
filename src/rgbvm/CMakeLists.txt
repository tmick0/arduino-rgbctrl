
generate_arduino_library(rgbvm_lib
    SRCS rgbvm.c
    HDRS rgbvm.h)

generate_arduino_firmware(rgbvm_main
    SRCS main.cpp
    LIBS rgbvm_lib)
