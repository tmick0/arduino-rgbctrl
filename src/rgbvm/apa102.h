#ifndef apa102_h_
#define apa102_h_

#include "driver.h"

#define DRIVER_BUFFER_SIZE 128

#ifdef __cplusplus
extern "C" {
#endif

void apa102_init(struct rgbvm_driver *driver, const uint8_t arg);
void apa102_write(struct rgbvm_driver *, const uint8_t r, const uint8_t g,
                  const uint8_t b);
void apa102_send(struct rgbvm_driver *driver);

#ifdef __cplusplus
}
#endif

#endif