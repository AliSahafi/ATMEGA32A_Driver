/*
 * ATmega32A Driver — UART Module
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_UART_HPP
#define ATMEGA32A_UART_HPP

#include "../common/common.hpp"
#include <stdlib.h> // itoa, utoa, ltoa, ultoa, dtostrf

// ---------------------------------------------------------
// UART Driver
// ---------------------------------------------------------
class UART_Driver {
private:
  static void (*rxInterruptCallback)(uint8_t);

public:
  static inline void init(uint32_t baudRate, bool enableRxInterrupt = false) {

    const bool doubleSpeed = true;
    uint16_t ubrr;

    if (doubleSpeed) {
      UCSRA |= (1 << U2X);
      ubrr = (F_CPU / 8 / baudRate) - 1;
    } else {
      UCSRA &= ~(1 << U2X);
      ubrr = (F_CPU / 16 / baudRate) - 1;
    }

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

static UART_Driver UART __attribute__((unused));

// UART RX Interrupt Service Routine
ISR(USART_RXC_vect) { UART.handleRxInterrupt(); }

#endif // ATMEGA32A_UART_HPP
