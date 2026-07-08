/*
 * ATmega32A Driver — SPI Module
 * Developed by Ali Sahafi <ali.sahafi@gmail.com> with help from Claude AI.
 */

#ifndef ATMEGA32A_SPI_HPP
#define ATMEGA32A_SPI_HPP

#include "../common/common.hpp"

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

static SPI_Driver SPI __attribute__((unused));

// SPI Interrupt Service Routine
ISR(SPI_STC_vect) { SPI.handleInterrupt(); }

#endif // ATMEGA32A_SPI_HPP
