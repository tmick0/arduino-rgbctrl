
#include "Arduino.h"
#include "rgbvm.h"

rgbvm_state vm;
unsigned char code[] = {0x01, 0x0f, 0x00, 0x11, 0x0f, 0xb1, 0x21, 0x0f,
                        0xff, 0x31, 0x00, 0x41, 0x02, 0x51, 0x02, 0x37,
                        0x54, 0x36, 0x54, 0x00, 0x02, 0x0f, 0x10, 0x31,
                        0x01, 0x41, 0x02, 0x51, 0x02, 0x37, 0x54, 0x36,
                        0x54, 0x01, 0x12, 0x0f, 0x10, 0x08, 0x09, 0x00};

void setup() {
  Serial.begin(9600);
  Serial.println("hello world");

  rgbvm_state_init(&vm, 40);
}

void output(unsigned char r, unsigned char g, unsigned char b,
            unsigned char o) {
  Serial.print("output ");
  Serial.print((int)o);
  Serial.print(": ");
  Serial.print((int)r);
  Serial.print(" ");
  Serial.print((int)g);
  Serial.print(" ");
  Serial.print((int)b);
  Serial.println("");
}

void loop() {
  if (rgbvm_apply(output, &vm, (const rgbvm_instruction *)&code[vm.ip]) !=
      RGBVM_STATUS_OK) {
    Serial.print("* FAILURE AT ip = ");
    Serial.println(vm.ip);
  }
}
