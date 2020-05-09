
#include <Arduino.h>
#include <EEPROM.h>

#include "ir.h"
#include "proto.h"
#include "rgbvm.h"

struct State {
  proto_state_machine psm;
  rgbvm_state vm;
  uint8_t code[128];
};

State s;

void setup() {
  Serial.begin(9600);

  const uint16_t code_len = (EEPROM.read(0) << 0) | (EEPROM.read(1) << 8);
  for (int i = 0; i < code_len; ++i) {
    s.code[i] = EEPROM.read(i + sizeof(uint16_t));
  }

  proto_state_machine_init(&s.psm);
  rgbvm_state_init(&s.vm, code_len);
  ir_setup(&s.vm);
}

void write_eeprom(const proto_state_machine *psm) {
  EEPROM.write(0, (s.vm.ip_max & (0x00ff)) >> 0);
  EEPROM.write(1, (s.vm.ip_max & (0xff00)) >> 8);
  for (int i = 0; i < s.vm.ip_max; ++i) {
    EEPROM.write(i + 2, s.code[i]);
  }
}

void loop() {
  if (Serial.available() > 0) {
    proto_msg msg;
    if (proto_state_machine_ingest(&s.vm, s.code, &s.psm, Serial.read(), &msg,
                                   write_eeprom)) {
      Serial.write((uint8_t)msg);
    }
  }

  if (s.psm.state == PROTO_STATE_INIT) {
    ir_decode(&s.vm);
    if (rgbvm_apply(delay, &s.vm,
                    (const rgbvm_instruction *)&s.code[s.vm.ip]) !=
        RGBVM_STATUS_OK) {
      Serial.print(F("* FAILURE AT ip = "));
      Serial.println(s.vm.ip);
    }
  }
}
