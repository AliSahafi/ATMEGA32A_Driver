/*
 * ATmega32A Driver
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_DRIVER_HPP
#define ATMEGA32A_DRIVER_HPP

#ifndef __AVR_ATmega32A__
#define __AVR_ATmega32A__
#endif

#ifndef F_CPU
#warning                                                                       \
    "F_CPU not defined! Please define it before including this driver or in your Makefile. Defaulting to 8MHz (8000000UL)."
#define F_CPU 8000000UL // Default to 8MHz if not defined
#endif

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

// ---------------------------------------------------------
// Seven Segment Display
// ---------------------------------------------------------
static inline uint8_t SevenSeg(uint8_t digit) {
  uint8_t seg_value;
  switch (digit) {
  case 0:
    seg_value = 0b10100000;
    break;
  case 1:
    seg_value = 0b11110011;
    break;
  case 2:
    seg_value = 0b10010100;
    break;
  case 3:
    seg_value = 0b10010001;
    break;
  case 4:
    seg_value = 0b11000011;
    break;
  case 5:
    seg_value = 0b10001001;
    break;
  case 6:
    seg_value = 0b10001000;
    break;
  case 7:
    seg_value = 0b10110011;
    break;
  case 8:
    seg_value = 0b10000000;
    break;
  case 9:
    seg_value = 0b10000001;
    break;
  default:
    seg_value = 0b10001100;
    break;
  }
  PORTB = seg_value;
  return seg_value;
}

// Arduino-like constants
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define ALL 0xFF

// ---------------------------------------------------------
// GPIO Driver
// ---------------------------------------------------------
class GPIO_Driver {
public:
  static inline void setDirection(volatile uint8_t &ddr, uint8_t pin,
                                  uint8_t dir) {
    volatile uint8_t *port = &ddr + 1; // In AVR, PORT is always DDR + 1
    if (pin == ALL) {
      if (dir == OUTPUT) {
        ddr = 0xFF;
      } else if (dir == INPUT_PULLUP) {
        ddr = 0x00;
        *port = 0xFF; // Enable all pull-ups
      } else {
        ddr = 0x00;
        *port = 0x00; // Disable all pull-ups
      }
    } else {
      if (dir == OUTPUT) {
        ddr |= (1 << pin);
      } else if (dir == INPUT_PULLUP) {
        ddr &= ~(1 << pin);
        *port |= (1 << pin); // Enable pull-up
      } else {
        ddr &= ~(1 << pin);
        *port &= ~(1 << pin); // Disable pull-up
      }
    }
  }

  static inline void write(volatile uint8_t &port, uint8_t pin, uint8_t state) {
    if (pin == ALL) {
      port = state;
    } else {
      if (state == HIGH) {
        port |= (1 << pin);
      } else {
        port &= ~(1 << pin);
      }
    }
  }

  static inline void write(volatile uint8_t &port, uint8_t value) {
    port = value;
  }

  static inline uint8_t read(volatile uint8_t &pin_reg, uint8_t pin) {
    if (pin == ALL)
      return pin_reg;
    return (pin_reg & (1 << pin)) ? HIGH : LOW;
  }

  static inline uint8_t read(volatile uint8_t &pin_reg) { return pin_reg; }

  static inline void toggle(volatile uint8_t &port, uint8_t pin) {
    if (pin == ALL) {
      port ^= 0xFF;
    } else {
      port ^= (1 << pin);
    }
  }
};

static GPIO_Driver GPIO;

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

static ADC_Driver ADC;

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

static PWM_Driver PWM;

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

static Timer_Driver Timer;

// ---------------------------------------------------------
// UART Driver
// ---------------------------------------------------------
class UART_Driver {
private:
  static void (*rxInterruptCallback)(uint8_t);

public:
  static inline void init(uint32_t baudRate, bool enableRxInterrupt = false) {
    uint16_t ubrr = (F_CPU / 16 / baudRate) - 1;
    UBRRH = (unsigned char)(ubrr >> 8);
    UBRRL = (unsigned char)ubrr;

    UCSRB = (1 << RXEN) | (1 << TXEN);

    if (enableRxInterrupt) {
      UCSRB |= (1 << RXCIE);
    }

    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
  }

  static inline void transmit(uint8_t data) {
    while (!(UCSRA & (1 << UDRE)))
      ;
    UDR = data;
  }

  static inline uint8_t receive() {
    while (!(UCSRA & (1 << RXC)))
      ;
    return UDR;
  }

  static inline void print(const char *str) {
    while (*str) {
      transmit(*str++);
    }
  }

  static inline void print(char c) { transmit(c); }

  static inline void print(int16_t value) {
    char buf[8];
    itoa(value, buf, 10);
    print(buf);
  }

  static inline void print(uint16_t value) {
    char buf[8];
    utoa(value, buf, 10);
    print(buf);
  }

  static inline void print(int32_t value) {
    char buf[12];
    ltoa(value, buf, 10);
    print(buf);
  }

  static inline void print(uint32_t value) {
    char buf[12];
    ultoa(value, buf, 10);
    print(buf);
  }

  static inline void print(double value, uint8_t decimalPlaces = 2) {
    char buf[16];
    dtostrf(value, 0, decimalPlaces, buf);
    print(buf);
  }

  static inline void attachRxInterrupt(void (*callback)(uint8_t)) {
    rxInterruptCallback = callback;
  }

  static inline void handleRxInterrupt() {
    uint8_t data = UDR;
    if (rxInterruptCallback) {
      rxInterruptCallback(data);
    }
  }
};

__attribute__((weak)) void (*UART_Driver::rxInterruptCallback)(uint8_t) =
    nullptr;

static UART_Driver UART;

// ---------------------------------------------------------
// SPI Driver
// ---------------------------------------------------------
#define SPI_MODE_0 0x00
#define SPI_MODE_1 0x04
#define SPI_MODE_2 0x08
#define SPI_MODE_3 0x0C

#define SPI_PRESCALER_4 0
#define SPI_PRESCALER_16 1
#define SPI_PRESCALER_64 2
#define SPI_PRESCALER_128 3
#define SPI_PRESCALER_2_2X 4
#define SPI_PRESCALER_8_2X 5
#define SPI_PRESCALER_32_2X 6
#define SPI_PRESCALER_64_2X 7

#define SPI_MSB_FIRST 0
#define SPI_LSB_FIRST 1

class SPI_Driver {
private:
  static void (*spiInterruptCallback)(uint8_t);

public:
  static inline void initMaster(uint8_t mode = SPI_MODE_0,
                                uint8_t prescaler = SPI_PRESCALER_4,
                                uint8_t dataOrder = SPI_MSB_FIRST,
                                bool enableInterrupt = false) {
    // Set MOSI (PB5), SCK (PB7) and SS (PB4) as output
    DDRB |= (1 << PB5) | (1 << PB7) | (1 << PB4);
    // Set MISO (PB6) as input
    DDRB &= ~(1 << PB6);

    uint8_t spcr = (1 << SPE) | (1 << MSTR) | mode | (prescaler & 0x03);
    if (dataOrder == SPI_LSB_FIRST) {
      spcr |= (1 << DORD);
    }
    if (enableInterrupt) {
      spcr |= (1 << SPIE);
    }

    if (prescaler >= SPI_PRESCALER_2_2X) {
      SPSR |= (1 << SPI2X);
    } else {
      SPSR &= ~(1 << SPI2X);
    }

    SPCR = spcr;
  }

  static inline void initSlave(uint8_t mode = SPI_MODE_0,
                               uint8_t dataOrder = SPI_MSB_FIRST,
                               bool enableInterrupt = false) {
    // Set MISO (PB6) as output
    DDRB |= (1 << PB6);
    // Set MOSI (PB5), SCK (PB7), SS (PB4) as input
    DDRB &= ~((1 << PB5) | (1 << PB7) | (1 << PB4));

    uint8_t spcr = (1 << SPE) | mode;
    if (dataOrder == SPI_LSB_FIRST) {
      spcr |= (1 << DORD);
    }
    if (enableInterrupt) {
      spcr |= (1 << SPIE);
    }

    SPCR = spcr;
  }

  static inline uint8_t transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
      ;
    return SPDR;
  }

  static inline void attachInterrupt(void (*callback)(uint8_t)) {
    spiInterruptCallback = callback;
  }

  static inline void handleInterrupt() {
    uint8_t data = SPDR;
    if (spiInterruptCallback) {
      spiInterruptCallback(data);
    }
  }
};

__attribute__((weak)) void (*SPI_Driver::spiInterruptCallback)(uint8_t) =
    nullptr;

static SPI_Driver SPI;

// UART RX Interrupt Service Routine
ISR(USART_RXC_vect) { UART.handleRxInterrupt(); }

// ADC Interrupt Service Routine
ISR(ADC_vect) { ADC.handleInterrupt(); }

// Timer Interrupt Service Routines
ISR(TIMER0_COMP_vect) { Timer.handleTimer0(); }
ISR(TIMER1_COMPA_vect) { Timer.handleTimer1(); }
ISR(TIMER2_COMP_vect) { Timer.handleTimer2(); }

// SPI Interrupt Service Routine
ISR(SPI_STC_vect) { SPI.handleInterrupt(); }

#endif // ATMEGA32A_DRIVER_HPP
