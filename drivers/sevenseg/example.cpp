/*
 * Seven-Segment Example -- count 0 to 9 forever
 *
 * Wiring:
 *   - Common-anode 7-segment display on PORTB (segments via 330R resistors)
 *   - The display is hardwired to PORTB in the driver
 *
 * Build & flash:  make sevenseg
 */

#define F_CPU 8000000UL
#include "../gpio/gpio.hpp"
#include "sevenseg.hpp"

int main() {
  GPIO.setDirection(DDRB, ALL, OUTPUT); // 7-segment lives on PORTB

  while (true) {
    for (uint8_t digit = 0; digit <= 9; digit++) {
      SevenSeg(digit);
      _delay_ms(500);
    }
  }

  return 0;
}
