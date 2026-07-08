#define F_CPU 8000000UL

// Include only the driver modules you need -- unused peripherals then cost
// no flash memory and no interrupt vectors.
#include "drivers/gpio/gpio.hpp"
// #include "drivers/sevenseg/sevenseg.hpp"
// #include "drivers/timer/timer.hpp"
// #include "drivers/pwm/pwm.hpp"
// #include "drivers/adc/adc.hpp"
// #include "drivers/uart/uart.hpp"
// #include "drivers/spi/spi.hpp"

int main() {

  while (true) {
  }

  return 0;
}
