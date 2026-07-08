/*
 * Timer Example -- blink an LED once per second using a Timer1 interrupt
 *
 * Timer1 (16-bit) in CTC mode:
 *   tick rate = F_CPU / (prescaler * (OCR + 1))
 *   8 MHz / (256 * (31249 + 1)) = 1 Hz  ->  interrupt fires every 1 second
 *
 * Wiring:
 *   - LED + 330R resistor from PC0 to GND
 *
 * Build & flash:  make timer
 */

#define F_CPU 8000000UL
#include "../gpio/gpio.hpp"
#include "timer.hpp"

void onOneSecond() {
  GPIO.toggle(PORTC, PC0); // runs in interrupt context -- keep it short!
}

int main() {
  GPIO.setDirection(DDRC, PC0, OUTPUT);

  Timer.initTimer1(31249, TIMER1_PRESCALER_256); // 1 second period
  Timer.attachTimer1(onOneSecond);

  sei(); // enable global interrupts

  while (true) {
    // Main loop is completely free -- the LED blinks in the background.
  }

  return 0;
}
