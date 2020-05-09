
generate_arduino_library(driver_lib
    SRCS driver.c analog.cpp ws2811.cpp
    HDRS driver.h analog.h   ws2811.h
)

generate_arduino_library(input_lib
    SRCS input.cpp
    HDRS input.h
)

generate_arduino_library(rgbvm_lib
    SRCS rgbvm.c
    HDRS rgbvm.h
    LIBS driver_lib input_lib
)

generate_arduino_library(proto_lib
    SRCS proto.c
    HDRS proto.h
    LIBS rgbvm_lib)

generate_arduino_firmware(rgbvm_main
    SRCS main.cpp ir.cpp
    LIBS rgbvm_lib proto_lib
    ARDLIBS EEPROM IRremote)
