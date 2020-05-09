#include "apa102.h"

#include <Arduino.h>

void apa102_init(struct rgbvm_driver *driver, const uint8_t arg) {
  driver->write = apa102_write;
  driver->send = apa102_send;
  driver->arg = arg;
  driver->ptr = 0;

  const uint8_t clock = arg, data = arg + 1;
  pinMode(clock, OUTPUT);
  pinMode(data, OUTPUT);
  digitalWrite(clock, LOW);
  digitalWrite(data, LOW);
}

void apa102_write(struct rgbvm_driver *driver, const uint8_t r, const uint8_t g,
                  const uint8_t b) {
  driver->buffer[driver->ptr].r = r;
  driver->buffer[driver->ptr].g = g;
  driver->buffer[driver->ptr].b = b;
  driver->ptr++;
}

static void apa102_byte(const uint8_t clock, const uint8_t data,
                        const uint8_t byte) {
  for (int i = 0; i < 8; ++i) {
    noInterrupts();
    digitalWrite(data, byte >> (7 - i) & 0x1);
    digitalWrite(clock, HIGH);
    digitalWrite(clock, LOW);
    interrupts();
  }
}

void apa102_send(struct rgbvm_driver *driver) {
  const uint8_t clock = driver->arg, data = driver->arg + 1;

  digitalWrite(data, LOW);

  // start frame
  apa102_byte(clock, data, 0);
  apa102_byte(clock, data, 0);
  apa102_byte(clock, data, 0);
  apa102_byte(clock, data, 0);

  for (int i = 0; i < driver->ptr; ++i) {

    // header (currently hardcoded max brightness)
    apa102_byte(clock, data, 0xff);

    // b, g, r
    apa102_byte(clock, data, driver->buffer[i].b);
    apa102_byte(clock, data, driver->buffer[i].g);
    apa102_byte(clock, data, driver->buffer[i].r);
  }

  // end frame
  for (int i = 0; i < driver->ptr; ++i) {
    apa102_byte(clock, data, 0x00);
  }

  digitalWrite(data, LOW);

  driver->ptr = 0;
}
