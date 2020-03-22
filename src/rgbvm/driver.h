#ifndef driver_h_
#define driver_h_

#include "driver.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

enum driver_type {
  DRIVER_UNSET = 0x0,
  DRIVER_ANALOG = 0x1
  // DRIVER_WS2812B = 0x2
  // DRIVER_APA102C = 0x3
};

struct rgbvm_driver;

typedef void (*driver_write)(struct rgbvm_driver *, const uint8_t,
                             const uint8_t, const uint8_t);
typedef void (*driver_init)(struct rgbvm_driver *, const uint8_t arg);
typedef void (*driver_send)(struct rgbvm_driver *);

struct rgb_value {
  uint8_t r, g, b;
};

struct rgbvm_driver {
  driver_write write;
  driver_send send;
  uint8_t arg;
  uint8_t ptr;
  struct rgb_value buffer[128];
};

driver_init get_driver(const enum driver_type);

#ifdef __cplusplus
}
#endif

#endif