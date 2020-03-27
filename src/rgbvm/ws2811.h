#ifndef ws2811_h_
#define ws2811_h_

#include "driver.h"

#ifdef __cplusplus
extern "C" {
#endif

void ws2811_init(struct rgbvm_driver *driver, const uint8_t arg);
void ws2811_write(struct rgbvm_driver *, const uint8_t r, const uint8_t g,
                  const uint8_t b);
void ws2811_send(struct rgbvm_driver *driver);

#ifdef __cplusplus
}
#endif

#endif