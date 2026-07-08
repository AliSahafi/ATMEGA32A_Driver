/*
 * PWM Example -- breathe (fade) an LED on PB3 using Timer0 Fast PWM
 *
 * Wiring:
 *   - LED + 330R resistor from PB3 (OC0) to GND
 *
 * Build & flash:  make pwm
 */

#define F_CPU 8000000UL
#include "pwm.hpp"

int main() {
  PWM.initTimer0_FastPWM(); // PB3 is configured as output automatically

  while (true) {
    // Fade up
    for (uint16_t duty = 0; duty <= 255; duty++) {
      PWM.setDutyCycleTimer0(duty);
      _delay_ms(4);
    }
    // Fade down
    for (int16_t duty = 255; duty >= 0; duty--) {
      PWM.setDutyCycleTimer0(duty);
      _delay_ms(4);
    }
  }

  return 0;
}
