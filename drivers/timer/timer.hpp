/*
 * ATmega32A Driver — Timer Module (Timer0/1/2, CTC interrupt mode)
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_TIMER_HPP
#define ATMEGA32A_TIMER_HPP

#include "../common/common.hpp"

// ---------------------------------------------------------
// Timer Driver
// ---------------------------------------------------------
#define TIMER0_PRESCALER_1 1
#define TIMER0_PRESCALER_8 2
#define TIMER0_PRESCALER_64 3
#define TIMER0_PRESCALER_256 4
#define TIMER0_PRESCALER_1024 5

#define TIMER1_PRESCALER_1 1
#define TIMER1_PRESCALER_8 2
#define TIMER1_PRESCALER_64 3
#define TIMER1_PRESCALER_256 4
#define TIMER1_PRESCALER_1024 5

// Timer2 has a different CS bit encoding than Timer0/1
#define TIMER2_PRESCALER_1 1
#define TIMER2_PRESCALER_8 2
#define TIMER2_PRESCALER_32 3
#define TIMER2_PRESCALER_64 4
#define TIMER2_PRESCALER_128 5
#define TIMER2_PRESCALER_256 6
#define TIMER2_PRESCALER_1024 7

class Timer_Driver {
private:
  static void (*timer0Callback)();
  static void (*timer1Callback)();
  static void (*timer2Callback)();
  static uint8_t timer0Prescaler;
  static uint8_t timer1Prescaler;
  static uint8_t timer2Prescaler;

public:
  // --- Timer 0 (8-bit CTC) ---
  // Fires every (prescaler * (ocr + 1)) / F_CPU seconds
  static inline void initTimer0(uint8_t ocr_val,
                                uint8_t prescaler = TIMER0_PRESCALER_64) {
    timer0Prescaler = prescaler & 0x07;
    TCCR0 = (1 << WGM01) | timer0Prescaler; // CTC mode
    OCR0 = ocr_val;
    TIMSK |= (1 << OCIE0);
  }

  static inline void attachTimer0(void (*callback)()) {
    timer0Callback = callback;
  }
  static inline void handleTimer0() {
    if (timer0Callback)
      timer0Callback();
  }

  static inline void stop0() { TCCR0 &= ~0x07; }
  static inline void start0() { TCCR0 |= timer0Prescaler; }
  static inline void reset0() { TCNT0 = 0; }
  static inline void restart0() {
    TCNT0 = 0;
    start0();
  }

  // --- Timer 1 (16-bit CTC) ---
  // Use OCR1A to set the compare value (up to 65535)
  static inline void initTimer1(uint16_t ocr_val,
                                uint8_t prescaler = TIMER1_PRESCALER_64) {
    timer1Prescaler = prescaler & 0x07;
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | timer1Prescaler; // CTC mode
    OCR1A = ocr_val;
    TIMSK |= (1 << OCIE1A);
  }

  static inline void attachTimer1(void (*callback)()) {
    timer1Callback = callback;
  }
  static inline void handleTimer1() {
    if (timer1Callback)
      timer1Callback();
  }

  static inline void stop1() { TCCR1B &= ~0x07; }
  static inline void start1() { TCCR1B |= timer1Prescaler; }
  static inline void reset1() { TCNT1 = 0; }
  static inline void restart1() {
    TCNT1 = 0;
    start1();
  }

  // --- Timer 2 (8-bit CTC) ---
  static inline void initTimer2(uint8_t ocr_val,
                                uint8_t prescaler = TIMER2_PRESCALER_64) {
    timer2Prescaler = prescaler & 0x07;
    TCCR2 = (1 << WGM21) | timer2Prescaler; // CTC mode
    OCR2 = ocr_val;
    TIMSK |= (1 << OCIE2);
  }

  static inline void attachTimer2(void (*callback)()) {
    timer2Callback = callback;
  }
  static inline void handleTimer2() {
    if (timer2Callback)
      timer2Callback();
  }

  static inline void stop2() { TCCR2 &= ~0x07; }
  static inline void start2() { TCCR2 |= timer2Prescaler; }
  static inline void reset2() { TCNT2 = 0; }
  static inline void restart2() {
    TCNT2 = 0;
    start2();
  }

  // --- Read current counter value ---
  static inline uint8_t read0() { return TCNT0; }
  static inline uint16_t read1() { return TCNT1; }
  static inline uint8_t read2() { return TCNT2; }

  // --- Blocking delay (no interrupt needed) ---
  static inline void delay_ms(uint16_t ms) {
    while (ms > 0) {
      _delay_ms(1);
      ms--;
    }
  }
};

__attribute__((weak)) void (*Timer_Driver::timer0Callback)() = nullptr;
__attribute__((weak)) void (*Timer_Driver::timer1Callback)() = nullptr;
__attribute__((weak)) void (*Timer_Driver::timer2Callback)() = nullptr;
__attribute__((weak)) uint8_t Timer_Driver::timer0Prescaler = 0;
__attribute__((weak)) uint8_t Timer_Driver::timer1Prescaler = 0;
__attribute__((weak)) uint8_t Timer_Driver::timer2Prescaler = 0;

static Timer_Driver Timer __attribute__((unused));

// Timer Interrupt Service Routines
ISR(TIMER0_COMP_vect) { Timer.handleTimer0(); }
ISR(TIMER1_COMPA_vect) { Timer.handleTimer1(); }
ISR(TIMER2_COMP_vect) { Timer.handleTimer2(); }

#endif // ATMEGA32A_TIMER_HPP
