#include "driver.h"
#include "analog.h"
#include "ws2811.h"

driver_init get_driver(const enum driver_type driver) {
  struct rgbvm_driver result;
  switch (driver) {
  case DRIVER_ANALOG:
    return analog_init;
  case DRIVER_WS2811:
    return ws2811_init;
  default:
    return 0;
  }
}
