#include "driver.h"
#include "analog.h"

driver_init get_driver(const enum driver_type driver) {
  struct rgbvm_driver result;
  switch (driver) {
  case DRIVER_ANALOG:
    return analog_init;
  default:
    return 0;
  }
}
