
generate_arduino_library(rgbvm_lib
    SRCS rgbvm.c
    HDRS rgbvm.h)

generate_arduino_library(proto_lib
    SRCS proto.c
    HDRS proto.h
    LIBS rgbvm_lib)

generate_arduino_firmware(rgbvm_main
    SRCS main.cpp
    LIBS rgbvm_lib proto_lib
    ARDLIBS EEPROM)
