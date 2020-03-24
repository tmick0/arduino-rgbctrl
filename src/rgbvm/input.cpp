#include "input.h"

#include <Arduino.h>

static int translate_pin(uint8_t pin) {
  switch (pin) {
  case 0:
    return A0;
  case 1:
    return A1;
  case 2:
    return A2;
  case 3:
    return A3;
  case 4:
    return A4;
  case 5:
    return A5;
  default:
    return -1;
  }
}

int input_read(const uint8_t pin, uint8_t *dest) {
  const int apin = translate_pin(pin);
  if (apin == -1) {
    return 1;
  }
  pinMode(apin, INPUT);
  *dest = map(analogRead(apin), 0, 1023, 0, 255);
  return 0;
}
