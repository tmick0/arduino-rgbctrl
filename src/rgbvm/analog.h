#ifndef analog_h_
#define analog_h_

#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

void analog_init(struct rgbvm_driver *driver, const uint8_t arg);
void analog_write(struct rgbvm_driver *, const uint8_t r, const uint8_t g,
                  const uint8_t b);
void analog_send(struct rgbvm_driver *driver);

#ifdef __cplusplus
}
#endif

#endif