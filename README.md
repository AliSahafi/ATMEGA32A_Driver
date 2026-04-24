# ATMEGA32A C++ Driver

A clean, lightweight, header-only C++ driver for the **ATMEGA32A** microcontroller.  
Designed for students and educators to get an Arduino-like development experience without any external frameworks.

---

## Features

| Module | Capabilities |
|---|---|
| **GPIO** | `OUTPUT`, `INPUT`, `INPUT_PULLUP` — per-pin or whole port at once |
| **ADC** | Blocking read, single-shot interrupt, free-running continuous mode |
| **UART** | TX/RX, interrupt-driven RX with callback, overloaded `print()` for all types |
| **Timers** | Timer0 (8-bit), Timer1 (16-bit), Timer2 (8-bit) — CTC interrupt mode |
| **PWM** | Fast-PWM on Timer0 (PB3) and Timer1 (PD4, PD5) |
| **Seven-Segment** | Built-in `SevenSeg(digit)` → auto-outputs to PORTB |

---

## Installation

### macOS
Install via [Homebrew](https://brew.sh):
```bash
brew tap osx-cross/avr
brew install avr-gcc avrdude
```

---

### Linux (Ubuntu / Debian)
```bash
sudo apt update
sudo apt install gcc-avr binutils-avr avr-libc avrdude make
```

For other distros (Arch, Fedora):
```bash
# Arch
sudo pacman -S avr-gcc avr-libc avrdude

# Fedora
sudo dnf install avr-gcc avr-libc avrdude
```

> **USBasp on Linux:** You may need to add a udev rule so `avrdude` can access the programmer without `sudo`. Create the file below and reload:
> ```bash
> echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", MODE="0666"' \
>   | sudo tee /etc/udev/rules.d/99-usbasp.rules
> sudo udevadm control --reload-rules && sudo udevadm trigger
> ```

---

### Windows

**Option A — WinAVR (simple, all-in-one installer)**
1. Download **WinAVR** from [https://winavr.sourceforge.net](https://winavr.sourceforge.net)
2. Run the installer — it adds `avr-g++`, `make`, and `avrdude` to your PATH automatically.

**Option B — Atmel/Microchip Toolchain + Zadig (recommended for newer boards)**
1. Download the **AVR 8-bit Toolchain** from [microchip.com](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio)
2. Download and install **avrdude for Windows** from [https://github.com/mariusgreuel/avrdude/releases](https://github.com/mariusgreuel/avrdude/releases)
3. Install **make** via [Chocolatey](https://chocolatey.org):
   ```powershell
   choco install make
   ```
4. Install the **USBasp driver** using [Zadig](https://zadig.akeo.ie):
   - Plug in your USBasp programmer
   - Open Zadig → select your USBasp device → install **WinUSB** driver

**Running `make` on Windows:**
Use either:
- **Git Bash** (comes with Git for Windows) — run `make` directly
- **PowerShell / Command Prompt** — requires `make` on PATH (Chocolatey installs it)

---

## Build & Flash

```bash
make
```

This compiles `main.cpp`, generates `main.hex`, and flashes it to the board.

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

```cpp
// Set direction
GPIO.setDirection(DDRB, PB0, OUTPUT);
GPIO.setDirection(DDRB, ALL, INPUT_PULLUP);  // Whole port with pull-ups

// Write
GPIO.write(PORTB, PB0, HIGH);
GPIO.write(PORTB, ALL, LOW);

// Read
uint8_t val = GPIO.read(PINB, PB0);   // Returns HIGH or LOW

// Toggle
GPIO.toggle(PORTB, PB0);
```

**Constants:** `INPUT`, `OUTPUT`, `INPUT_PULLUP`, `HIGH`, `LOW`, `ALL`

---

### ADC

```cpp
// Blocking (polling) read
ADC.init(ADC_REF_AVCC, ADC_PRESCALER_64);
uint16_t val = ADC.read(ADC_CHANNEL_0);   // Returns 0–1023

// Interrupt-driven (single shot)
ADC.init(ADC_REF_AVCC, ADC_PRESCALER_64, true);
ADC.attachInterrupt(myCallback);           // void myCallback(uint16_t value)
ADC.startConversion(ADC_CHANNEL_0);
sei();

// Continuous free-running (fastest, fully automatic)
ADC.startContinuous(ADC_CHANNEL_0);
ADC.stopContinuous();
```

**Reference:** `ADC_REF_AREF`, `ADC_REF_AVCC`, `ADC_REF_INTERNAL`  
**Prescaler:** `ADC_PRESCALER_2/4/8/16/32/64/128`  
**Channel:** `ADC_CHANNEL_0` … `ADC_CHANNEL_7`

---

### UART

```cpp
UART.init(9600);                      // Polling mode
UART.init(9600, true);                // RX interrupt enabled

UART.transmit('A');                   // Send raw byte
UART.print("Hello!\r\n");            // String
UART.print(3.14f, 2);                // Float with decimal places
UART.print((uint16_t)1023);          // Integer (all sizes supported)

uint8_t byte = UART.receive();        // Blocking receive

// Interrupt-driven RX callback
UART.attachRxInterrupt(myCallback);   // void myCallback(uint8_t byte)
sei();
```

---

### Timers (CTC Interrupt Mode)

```cpp
// Timer 0 — 8-bit, OCR max 255
// Tick rate: F_CPU / (prescaler * (ocr + 1))
Timer.initTimer0(124, TIMER_PRESCALER_64);    // 1ms tick at 8MHz
Timer.attachTimer0(myCallback);               // void myCallback()

// Timer 1 — 16-bit, OCR max 65535
Timer.initTimer1(31249, TIMER_PRESCALER_256); // 1s tick at 8MHz
Timer.attachTimer1(myCallback);

// Timer 2 — 8-bit (uses TIMER2_PRESCALER_xxx!)
Timer.initTimer2(243, TIMER2_PRESCALER_1024); // ~31ms tick at 8MHz
Timer.attachTimer2(myCallback);

// Blocking delay
Timer.delay_ms(500);

sei();  // Always call after setup to enable interrupts
```

> ⚠️ **Timer prescaler note:** Timer2 uses a different prescaler encoding than Timer0/1.  
> Always use `TIMER2_PRESCALER_xxx` for Timer2 and `TIMER_PRESCALER_xxx` for Timer0/1.

> ⚠️ **PWM conflict:** PWM uses Timer0 and Timer1 internally.  
> Do NOT use `Timer.initTimer0()` and `PWM.initTimer0_FastPWM()` at the same time.

---

### PWM

```cpp
// Timer 0 Fast-PWM → output pin: PB3
PWM.initTimer0_FastPWM();
PWM.setDutyCycleTimer0(128);          // 0–255 (50% duty cycle)

// Timer 1 Fast-PWM 8-bit → output pins: PD4, PD5
PWM.initTimer1_FastPWM_8bit();
PWM.setDutyCycleTimer1A(200);
PWM.setDutyCycleTimer1B(50);
```

---

### Seven-Segment Display

The display is hardwired to **PORTB**. Just call:

```cpp
GPIO.setDirection(DDRB, ALL, OUTPUT);
SevenSeg(7);   // Displays the digit "7" on the connected 7-segment display
```

Supports digits `0–9`. Any other value shows the letter `E` (error).

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

    Timer.initTimer1(31249, TIMER_PRESCALER_256);
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

**Ali Sahafi** — ali.sahafi@gmail.com
