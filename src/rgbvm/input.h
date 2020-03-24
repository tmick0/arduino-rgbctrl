#ifndef input_h_
#define input_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

int input_read(const uint8_t pin, uint8_t *dest);

#ifdef __cplusplus
}
#endif

#endif
