/*
 * ATmega32A Driver — ADC Module
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_ADC_HPP
#define ATMEGA32A_ADC_HPP

#include "../common/common.hpp"

// ---------------------------------------------------------
// ADC Driver
// ---------------------------------------------------------
#undef ADC
#define ADC_REF_AREF 0
#define ADC_REF_AVCC 1
#define ADC_REF_INTERNAL 3

#define ADC_PRESCALER_2 1
#define ADC_PRESCALER_4 2
#define ADC_PRESCALER_8 3
#define ADC_PRESCALER_16 4
#define ADC_PRESCALER_32 5
#define ADC_PRESCALER_64 6
#define ADC_PRESCALER_128 7

#define ADC_CHANNEL_0 0
#define ADC_CHANNEL_1 1
#define ADC_CHANNEL_2 2
#define ADC_CHANNEL_3 3
#define ADC_CHANNEL_4 4
#define ADC_CHANNEL_5 5
#define ADC_CHANNEL_6 6
#define ADC_CHANNEL_7 7

class ADC_Driver {
private:
  static void (*adcInterruptCallback)(uint16_t);

public:
  static inline void init(uint8_t ref = ADC_REF_AVCC,
                          uint8_t prescaler_bits = ADC_PRESCALER_64,
                          bool enableInterrupt = false) {
    ADMUX &= ~((1 << REFS1) | (1 << REFS0));
    if (ref == ADC_REF_AVCC) {
      ADMUX |= (1 << REFS0);
    } else if (ref == ADC_REF_INTERNAL) {
      ADMUX |= (1 << REFS1) | (1 << REFS0);
    }

    ADCSRA = (1 << ADEN) | (prescaler_bits & 0x07);
    if (enableInterrupt) {
      ADCSRA |= (1 << ADIE);
    }
  }

  static inline uint16_t read(uint8_t channel = 0) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
      ;
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    return (high << 8) | low;
  }

  static inline void startConversion(uint8_t channel = 0) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
  }

  static inline void startContinuous(uint8_t channel = 0) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
    ADCSRA |= (1 << ADATE); // Enable Auto Triggering
    SFIOR &= ~((1 << ADTS2) | (1 << ADTS1) |
               (1 << ADTS0)); // Set to Free Running Mode
    ADCSRA |= (1 << ADSC);    // Start the first conversion
  }

  static inline void stopContinuous() { ADCSRA &= ~(1 << ADATE); }

  static inline void stop() { ADCSRA &= ~(1 << ADEN); }
  static inline void start() { ADCSRA |= (1 << ADEN); }

  static inline bool isReady() { return !(ADCSRA & (1 << ADSC)); }

  static inline uint16_t lastResult() {
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    return (high << 8) | low;
  }

  static inline void attachInterrupt(void (*callback)(uint16_t)) {
    adcInterruptCallback = callback;
  }

  static inline void handleInterrupt() {
    uint8_t low = ADCL;
    uint8_t high = ADCH;
    uint16_t val = (high << 8) | low;
    if (adcInterruptCallback) {
      adcInterruptCallback(val);
    }
  }
};

__attribute__((weak)) void (*ADC_Driver::adcInterruptCallback)(uint16_t) =
    nullptr;

static ADC_Driver ADC __attribute__((unused));

// ADC Interrupt Service Routine
ISR(ADC_vect) { ADC.handleInterrupt(); }

#endif // ATMEGA32A_ADC_HPP
