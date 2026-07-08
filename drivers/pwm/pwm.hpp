/*
 * ATmega32A Driver — PWM Module (Fast PWM on Timer0/Timer1)
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_PWM_HPP
#define ATMEGA32A_PWM_HPP

#include "../common/common.hpp"

// ---------------------------------------------------------
// PWM Driver
// ---------------------------------------------------------
class PWM_Driver {
public:
  static inline void initTimer0_FastPWM() {
    DDRB |= (1 << PB3);
    TCCR0 = (1 << WGM00) | (1 << WGM01) | (1 << COM01) | (1 << CS01);
  }

  static inline void setDutyCycleTimer0(uint8_t duty) { OCR0 = duty; }

  static inline void initTimer1_FastPWM_8bit() {
    DDRD |= (1 << PD4) | (1 << PD5);
    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
    TCCR1B = (1 << WGM12) | (1 << CS11);
  }

  static inline void setDutyCycleTimer1A(uint8_t duty) { OCR1A = duty; }
  static inline void setDutyCycleTimer1B(uint8_t duty) { OCR1B = duty; }
};

static PWM_Driver PWM __attribute__((unused));

#endif // ATMEGA32A_PWM_HPP
