#include "ir.h"

#include <EEPROM.h>
#include <IRremote.h>

enum ir_codes {
  ir_code_add = 0xFFA857,
  ir_code_sub = 0xFFE01F,
  ir_code_chan1 = 0xFF30CF,
  ir_code_chan2 = 0xFF18E7,
  ir_code_chan3 = 0xFF7A85,
  ir_code_chan4 = 0xFF10EF,
  ir_code_chan5 = 0xFF38C7,
  ir_code_chan6 = 0xFF5AA5,
  ir_code_chan7 = 0xFF42BD,
  ir_code_chan8 = 0xFF4AB5,
  ir_code_chan9 = 0xFF52AD
};

IRrecv irrecv(ir_pin);

void ir_setup(struct rgbvm_state *vm) {
  const size_t addr = vm->ip_max + sizeof(uint16_t);
  for (int i = 0; i < 9; ++i) {
    vm->remote_values[i] = EEPROM.read(addr + i);
  }
  irrecv.enableIRIn();
}

int ir_active_chan = 0;
unsigned long ir_last_code = ir_code_chan1;

static void ir_update(struct rgbvm_state *vm, const uint8_t channel,
                      const int8_t update) {
  if (channel > 9) {
    return;
  }
  vm->remote_values[channel] += update;
  const size_t addr = vm->ip_max + sizeof(uint16_t);
  EEPROM.write(addr + channel, vm->remote_values[channel]);
}

void ir_decode(struct rgbvm_state *vm) {
  decode_results res;
  if (irrecv.decode(&res)) {
    unsigned long code = res.value;
    if (code == 0xffffffff) {
      code = ir_last_code;
    }
    ir_last_code = code;
    switch (code) {
    case ir_code_add: {
      ir_update(vm, ir_active_chan, +1);
      break;
    }
    case ir_code_sub: {
      ir_update(vm, ir_active_chan, -1);
      break;
    }
    case ir_code_chan1: {
      ir_active_chan = 0;
      break;
    }
    case ir_code_chan2: {
      ir_active_chan = 1;
      break;
    }
    case ir_code_chan3: {
      ir_active_chan = 2;
      break;
    }
    case ir_code_chan4: {
      ir_active_chan = 3;
      break;
    }
    case ir_code_chan5: {
      ir_active_chan = 4;
      break;
    }
    case ir_code_chan6: {
      ir_active_chan = 5;
      break;
    }
    case ir_code_chan7: {
      ir_active_chan = 6;
      break;
    }
    case ir_code_chan8: {
      ir_active_chan = 7;
      break;
    }
    case ir_code_chan9: {
      ir_active_chan = 8;
      break;
    }
    }
    irrecv.resume();
  }
}
