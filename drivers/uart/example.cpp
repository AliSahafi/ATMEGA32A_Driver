/*
 * UART Example -- serial echo with RX interrupt + print() showcase
 *
 * Open a serial terminal at 9600 baud (8N1). Every character you type is
 * echoed back by the RX interrupt, and a counter is printed once per second.
 *
 * Wiring:
 *   - PD0 (RXD) -> TX of your USB-serial adapter
 *   - PD1 (TXD) -> RX of your USB-serial adapter
 *   - GND <-> GND
 *
 * Build & flash:  make uart
 */

#define F_CPU 8000000UL
#include "uart.hpp"

void onReceive(uint8_t byte) {
  UART.print("Echo: ");
  UART.transmit(byte);
  UART.print("\r\n");
}

int main() {
  UART.init(9600, true); // true = enable RX interrupt
  UART.attachRxInterrupt(onReceive);
  sei();

  UART.print("UART ready! Type something...\r\n");

  uint16_t seconds = 0;
  while (true) {
    _delay_ms(1000);
    seconds++;

    // print() is overloaded for strings, chars, ints and floats:
    UART.print("Uptime: ");
    UART.print(seconds);
    UART.print(" s  (");
    UART.print(seconds / 60.0, 2);
    UART.print(" min)\r\n");
  }

  return 0;
}
