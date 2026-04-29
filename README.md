# ATMEGA32A C++ Driver

A clean, lightweight, header-only C++ driver for the **ATMEGA32A** microcontroller.  
Designed for students and educators to get an Arduino-like development experience without any external frameworks.

---

## Getting Started

### Step 1: Install the AVR Toolchain

### Ubuntu / Debian
To install the AVR GCC toolchain, `avrdude`, and `make`:
```bash
sudo apt-get update && sudo apt-get install -y gcc-avr binutils-avr avr-libc avrdude make
```

### macOS
Using [Homebrew](https://brew.sh/):
```bash
brew tap osx-cross/avr
brew install avr-gcc avrdude make
```

### Windows
Using [Scoop](https://scoop.sh/) (Recommended):
```powershell
scoop install avr-gcc avrdude make
```

Or using [Chocolatey](https://chocolatey.org/):
```powershell
choco install avr-gcc avrdude make
```

> 💡 **Windows USBasp Users:** If using a USBasp programmer, Windows will not recognize it out of the box. You have two options:
> 1. **Use the bundled driver (easiest):** Extract [`USBasp-win-driver-x86-x64-v3.0.7.zip`](USBasp-win-driver-x86-x64-v3.0.7.zip) included in this repo and run `InstallDriver.exe` (matching your CPU architecture).
> 2. **Use Zadig:** Download [Zadig](https://zadig.akeo.ie/), plug in your USBasp, and replace its driver with `libusb-win32`.

### Step 2: Build & Flash

Navigate to the project directory and run:
```bash
make
```

This will compile and flash your ATMEGA32A using your configured programmer.

---

## Features

| Module | Capabilities |
|---|---|
| **GPIO** | `OUTPUT`, `INPUT`, `INPUT_PULLUP` — per-pin or whole port at once |
| **ADC** | Blocking read, single-shot interrupt, free-running continuous mode |
| **UART** | TX/RX, interrupt-driven RX with callback, overloaded `print()` for all types |
| **Timers** | Timer0 (8-bit), Timer1 (16-bit), Timer2 (8-bit) — CTC interrupt mode |
| **PWM** | Fast-PWM on Timer0 (PB3) and Timer1 (PD4, PD5) |
| **SPI** | Master/Slave mode, hardware transfer, and STC interrupt |
| **Seven-Segment** | Built-in `SevenSeg(digit)` → auto-outputs to PORTB |

---

## Driver Reference

### Setup

```cpp
#define F_CPU 8000000UL     // Define BEFORE including the driver!
#include "atmega32a_driver.hpp"
```

> ⚠️ If `F_CPU` is not defined, the driver defaults to 8MHz and shows a compiler warning.

---

### GPIO

| Method | Description |
|---|---|
| `GPIO.setDirection(DDRx, pin, OUTPUT)` | Set single pin direction |
| `GPIO.setDirection(DDRx, ALL, INPUT_PULLUP)` | Set whole port direction |
| `GPIO.write(PORTx, pin, HIGH/LOW)` | Write single pin |
| `GPIO.write(PORTx, ALL, 0xF0)` | Write raw byte to whole port |
| `GPIO.write(PORTx, 0xF0)` | Shorthand whole-port write |
| `GPIO.read(PINx, pin)` | Read single pin — returns `HIGH` or `LOW` |
| `GPIO.read(PINx, ALL)` | Read whole port — returns 0–255 |
| `GPIO.read(PINx)` | Shorthand whole-port read |
| `GPIO.toggle(PORTx, pin)` | Toggle single pin |
| `GPIO.toggle(PORTx, ALL)` | Toggle whole port |

**Constants:** `INPUT`, `OUTPUT`, `INPUT_PULLUP`, `HIGH`, `LOW`, `ALL`

---

### ADC

| Method | Description |
|---|---|
| `ADC.init(ref, prescaler, interrupt)` | Configure reference, prescaler, optional interrupt |
| `ADC.read(channel)` | Blocking read — waits for conversion, returns 0–1023 |
| `ADC.startConversion(channel)` | Non-blocking single-shot conversion |
| `ADC.startContinuous(channel)` | Free-running mode — auto-converts continuously |
| `ADC.stopContinuous()` | Stop free-running mode |
| `ADC.isReady()` | Returns `true` when conversion is complete |
| `ADC.lastResult()` | Read last result without triggering a new conversion |
| `ADC.stop()` | Disable ADC |
| `ADC.start()` | Re-enable ADC |
| `ADC.attachInterrupt(callback)` | Set conversion callback: `void cb(uint16_t value)` |

**Reference:** `ADC_REF_AREF`, `ADC_REF_AVCC`, `ADC_REF_INTERNAL`  
**Prescaler:** `ADC_PRESCALER_2/4/8/16/32/64/128`  
**Channel:** `ADC_CHANNEL_0` … `ADC_CHANNEL_7`

---

### UART

| Method | Description |
|---|---|
| `UART.init(baud, interrupt)` | Configure baud rate, optional RX interrupt |
| `UART.transmit(byte)` | Send raw byte |
| `UART.print(value)` | Send string, char, int, uint, float (all types) |
| `UART.receive()` | Blocking receive — waits for incoming byte |
| `UART.attachRxInterrupt(callback)` | Set RX callback: `void cb(uint8_t byte)` |

---

### Timers (CTC Interrupt Mode)

Tick rate: `F_CPU / (prescaler × (ocr + 1))`

| Method | Description |
|---|---|
| `Timer.initTimer0(ocr, prescaler)` | 8-bit CTC, OCR max 255 |
| `Timer.initTimer1(ocr, prescaler)` | 16-bit CTC, OCR max 65535 |
| `Timer.initTimer2(ocr, prescaler)` | 8-bit CTC, OCR max 255 |
| `Timer.attachTimer0/1/2(callback)` | Set callback: `void cb()` |
| `Timer.stop0/1/2()` | Halt counter (preserves config) |
| `Timer.start0/1/2()` | Resume from current count |
| `Timer.reset0/1/2()` | Set counter to 0 (keeps running) |
| `Timer.restart0/1/2()` | Set counter to 0 and start |
| `Timer.read0/2()` | Read 8-bit counter (0–255) |
| `Timer.read1()` | Read 16-bit counter (0–65535) |
| `Timer.delay_ms(ms)` | Blocking delay |

**Timer0/1 Prescaler:** `TIMER0_PRESCALER_1/8/64/256/1024` — `TIMER1_PRESCALER_1/8/64/256/1024`  
**Timer2 Prescaler:** `TIMER2_PRESCALER_1/8/32/64/128/256/1024` *(different encoding — always use `TIMER2_PRESCALER_xxx` for Timer2)*

> ⚠️ **PWM conflict:** Do NOT use `Timer.initTimer0/1()` and `PWM.initTimer0/1_FastPWM()` at the same time.

---

### PWM

| Method | Description |
|---|---|
| `PWM.initTimer0_FastPWM()` | Fast-PWM on PB3 |
| `PWM.setDutyCycleTimer0(duty)` | Set Timer0 duty cycle, 0–255 |
| `PWM.initTimer1_FastPWM_8bit()` | Fast-PWM on PD4 (OC1B) and PD5 (OC1A) |
| `PWM.setDutyCycleTimer1A(duty)` | Set PD5 duty cycle, 0–255 |
| `PWM.setDutyCycleTimer1B(duty)` | Set PD4 duty cycle, 0–255 |

---

### SPI

| Method | Description |
|---|---|
| `SPI.initMaster(mode, prescaler, dataOrder, interrupt)` | Configure as SPI Master |
| `SPI.initSlave(mode, dataOrder, interrupt)` | Configure as SPI Slave |
| `SPI.transfer(data)` | Synchronous block transfer (returns received byte) |
| `SPI.attachInterrupt(callback)` | Set transfer complete callback: `void cb(uint8_t data)` |

**Mode:** `SPI_MODE_0/1/2/3`  
**Prescaler:** `SPI_PRESCALER_4/16/64/128`, `SPI_PRESCALER_2_2X/8_2X/32_2X/64_2X`  
**Data Order:** `SPI_MSB_FIRST`, `SPI_LSB_FIRST`

> 💡 **Pin config:** The `initMaster()` and `initSlave()` methods automatically configure the DDR registers for PB4(SS), PB5(MOSI), PB6(MISO), and PB7(SCK).

---

### Seven-Segment Display

| Method | Description |
|---|---|
| `SevenSeg(digit)` | Display digit 0–9 on PORTB; any other value shows `E` |

Hardwired to **PORTB** — set `DDRB` to `OUTPUT` before calling.

---

## Full Interrupt Example

```cpp
#define F_CPU 8000000UL
#include "atmega32a_driver.hpp"

volatile uint16_t ADC_result = 0;

void onADCReady(uint16_t value)  { ADC_result = value; SevenSeg(value / 103); }
void onUARTReceive(uint8_t byte) { UART.print("Echo: "); UART.transmit(byte); UART.print("\r\n"); }
void onTimer1()                  { UART.print("ADC = "); UART.print(ADC_result); UART.print("\r\n"); }

int main() {
    UART.init(9600, true);
    UART.attachRxInterrupt(onUARTReceive);

    GPIO.setDirection(DDRA, ALL, INPUT);
    GPIO.setDirection(DDRB, ALL, OUTPUT);

    ADC.init(ADC_REF_AVCC, ADC_PRESCALER_64, true);
    ADC.attachInterrupt(onADCReady);
    ADC.startContinuous(ADC_CHANNEL_0);

    Timer.initTimer1(31249, TIMER1_PRESCALER_256);
    Timer.attachTimer1(onTimer1);

    sei();
    UART.print("System ready!\r\n");

    while (true) { /* main loop free for application logic */ }
}
```

---

## File Structure

```
main/
├── atmega32a_driver.hpp   # The complete driver (header-only)
├── main.cpp               # Your application code
├── Makefile               # Build & flash pipeline
└── README.md              # This file
```

---

## Author

**Ali Sahafi** — ali.sahafi@gmail.com with help of Claude :)
