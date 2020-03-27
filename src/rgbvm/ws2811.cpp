#include "ws2811.h"

#include <Arduino.h>

void ws2811_init(struct rgbvm_driver *driver, const uint8_t arg) {
  driver->arg = arg;
  driver->ptr = 0;
  driver->write = ws2811_write;
  driver->send = ws2811_send;
  pinMode(arg, OUTPUT);
  digitalWrite(arg, 0);
}

void ws2811_write(struct rgbvm_driver *driver, const uint8_t r, const uint8_t g,
                  const uint8_t b) {
  if (driver->ptr < DRIVER_BUFFER_SIZE) {
    driver->buffer[driver->ptr].r = g;
    driver->buffer[driver->ptr].g = r;
    driver->buffer[driver->ptr].b = b;
    driver->ptr++;
  }
}

void ws2811_send(struct rgbvm_driver *driver) {

  // adapted from code found at
  // https://www.instructables.com/id/Bitbanging-step-by-step-Arduino-control-of-WS2811-/

  const uint8_t PORT_NUM = digitalPinToPort(driver->arg);
  const uint8_t PORT_MASK = digitalPinToBitMask(driver->arg);
  volatile uint8_t *PORT = portOutputRegister(PORT_NUM);

  cli();

  const uint32_t t = micros();
  while ((micros() - t) < 50L) {
  }

  volatile uint8_t *p = (uint8_t *)driver->buffer;
  volatile uint8_t val = *p++;
  volatile uint8_t high = *PORT | PORT_MASK;
  volatile uint8_t low = *PORT & ~PORT_MASK;
  volatile uint8_t tmp = low;
  volatile uint8_t nbits = 8;
  volatile uint16_t nbytes = 3 * driver->ptr;

  asm volatile("nextbit:\n\t"
               "st %a0, %1\n\t"
               "sbrc %2, 7\n\t"
               "mov  %4, %1\n\t"
               "dec  %3\n\t"
               "nop\n\t"
               "st   %a0, %4\n\t"
               "mov  %4, %5\n\t"
               "breq nextbyte\n\t"
               "rol  %2\n\t"
               "rjmp .+0\n\t"
               "st %a0, %5\n\t"
               "rjmp .+0\n\t"
               "nop\n\t"
               "rjmp nextbit\n\t"
               "nextbyte:\n\t"
               "ldi  %3, 8\n\t"
               "ld   %2, %a6+\n\t"
               "st %a0, %5\n\t"
               "rjmp .+0\n\t"
               "nop\n\t"
               "dec %7\n\t"
               "brne nextbit\n\t" ::"e"(PORT),
               "r"(high), "r"(val), "r"(nbits), "r"(tmp), "r"(low), "e"(p),
               "w"(nbytes));
  sei();

  driver->ptr = 0;
}