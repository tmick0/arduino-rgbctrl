
#include <Arduino.h>
#include <EEPROM.h>

#include "proto.h"
#include "rgbvm.h"

struct State {
  proto_state_machine psm;
  rgbvm_state vm;
  uint8_t code[128];
};

State s;

const int rpin = 9;
const int gpin = 10;
const int bpin = 11;

void setup() {
  Serial.begin(9600);
  pinMode(rpin, OUTPUT);
  pinMode(gpin, OUTPUT);
  pinMode(bpin, OUTPUT);

  const uint16_t code_len = (EEPROM.read(0) << 0) | (EEPROM.read(1) << 8);
  for (int i = 0; i < code_len; ++i) {
    s.code[i] = EEPROM.read(i + sizeof(uint16_t));
  }

  proto_state_machine_init(&s.psm);
  rgbvm_state_init(&s.vm, code_len);
}

void output(uint8_t r, uint8_t g, uint8_t b, uint8_t o) {
  if (o == 0) {
    analogWrite(rpin, r);
    analogWrite(gpin, g);
    analogWrite(bpin, b);
  }
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
    if (rgbvm_apply(delay, output, &s.vm,
                    (const rgbvm_instruction *)&s.code[s.vm.ip]) !=
        RGBVM_STATUS_OK) {
      Serial.print("* FAILURE AT ip = ");
      Serial.println(s.vm.ip);
    }
  }
}
