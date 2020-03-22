#include "analog.h"

#include <Arduino.h>

const int rpin = 9;
const int gpin = 10;
const int bpin = 11;

void analog_init(struct rgbvm_driver *driver, const uint8_t arg) {
  driver->write = analog_write;
  driver->send = analog_send;
  driver->arg = arg;
  driver->ptr = 0;
  pinMode(rpin, OUTPUT);
  pinMode(gpin, OUTPUT);
  pinMode(bpin, OUTPUT);
}

void analog_write(struct rgbvm_driver *driver, const uint8_t r, const uint8_t g,
                  const uint8_t b) {
  driver->buffer[0].r = r;
  driver->buffer[0].g = g;
  driver->buffer[0].b = b;
}

void analog_send(struct rgbvm_driver *driver) {
  analogWrite(rpin, driver->buffer[0].r);
  analogWrite(gpin, driver->buffer[0].g);
  analogWrite(bpin, driver->buffer[0].b);
}
