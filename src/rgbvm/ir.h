#ifndef ir_h_
#define ir_h_

#include "rgbvm.h"

const int ir_pin = 5;

void ir_setup(struct rgbvm_state *vm);
void ir_decode(struct rgbvm_state *vm);

#endif