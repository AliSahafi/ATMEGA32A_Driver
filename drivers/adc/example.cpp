/*
 * ADC Example -- potentiometer bar graph
 *
 * Reads a potentiometer on ADC0 (PA0) with a blocking read and shows the
 * value as a bar of 0-8 LEDs on PORTC.
 *
 * Wiring:
 *   - Potentiometer: outer pins to VCC and GND, wiper to PA0
 *   - 8 LEDs + 330R resistors from PC0..PC7 to GND
 *   - AVCC connected to VCC (the ADC is powered from the AVCC pin!)
 *
 * Build & flash:  make adc
 */

#define F_CPU 8000000UL
#include "../gpio/gpio.hpp"
#include "adc.hpp"

int main() {
  GPIO.setDirection(DDRA, PA0, INPUT); // ADC0 input
  GPIO.setDirection(DDRC, ALL, OUTPUT); // LED bar

  ADC.init(ADC_REF_AVCC, ADC_PRESCALER_64);

  while (true) {
    uint16_t value = ADC.read(ADC_CHANNEL_0); // 0..1023

    // Map 0..1023 to 0..255 for 8 LEDs
    uint8_t bar = (value * 255) / 1023;
    GPIO.write(PORTC, bar);

    _delay_ms(50);
  }

  return 0;
}
