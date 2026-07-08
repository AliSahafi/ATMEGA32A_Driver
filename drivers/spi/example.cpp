/*
 * SPI Example -- master sends a counter, prints what comes back over UART
 *
 * Simplest test: connect MOSI to MISO with a jumper wire (loopback) and
 * every byte sent comes straight back. With a real slave device, the
 * returned byte is whatever the slave shifted out.
 *
 * Wiring:
 *   - PB5 (MOSI) -> slave MOSI  (or jumper to PB6 for loopback)
 *   - PB6 (MISO) <- slave MISO
 *   - PB7 (SCK)  -> slave SCK
 *   - PB4 (SS)   -> slave SS
 *   - UART on PD0/PD1 at 9600 baud to see the results
 *
 * Build & flash:  make spi
 */

#define F_CPU 8000000UL
#include "../uart/uart.hpp"
#include "spi.hpp"

int main() {
  UART.init(9600);
  SPI.initMaster(SPI_MODE_0, SPI_PRESCALER_16); // pins configured automatically

  UART.print("SPI master ready (loopback test: jumper MOSI-MISO)\r\n");

  uint8_t counter = 0;
  while (true) {
    uint8_t received = SPI.transfer(counter);

    UART.print("Sent: ");
    UART.print((uint16_t)counter);
    UART.print("  Received: ");
    UART.print((uint16_t)received);
    UART.print(received == counter ? "  OK\r\n" : "  MISMATCH\r\n");

    counter++;
    _delay_ms(500);
  }

  return 0;
}
