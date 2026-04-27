#define F_CPU 8000000UL
#include "atmega32a_driver.hpp"

volatile uint16_t ADC_result = 0;

// ---- ADC Interrupt ----
// Called automatically every time the ADC finishes a conversion
void onADCReady(uint16_t value) {
  SevenSeg(value / 103);          // Map 0-1023 to 0-9 on the display
  ADC_result = value;             // Store latest ADC value for main loop or other interrupts
}

// ---- UART RX Interrupt ----
// Called automatically when a byte arrives over serial
void onUARTReceive(uint8_t byte) {
  UART.print("Echo: ");
  UART.transmit(byte);
  UART.print("\r\n");
}

// ---- Timer 0 Interrupt (8-bit, fast ticks) ----
// Fires every 1ms  →  OCR0 = (8MHz / 64 / 1000) - 1 = 124
volatile uint16_t ms_counter = 0;
void onTimer0() {
  ms_counter++;
}

// ---- Timer 1 Interrupt (16-bit, 1 second) ----
// Fires every 1s  →  OCR1A = (8MHz / 256 / 1) - 1 = 31249
void onTimer1() {
  UART.print("ADC = ");
  UART.print(ADC_result);
  UART.print("\r\n");
}

// ---- Timer 2 Interrupt (8-bit, LED blink) ----
// Fires every 500ms  →  OCR2 = (8MHz / 1024 / 2) - 1 = 3905  — too big for 8-bit!
// Use prescaler 1024, fire every ~31ms, count 16 ticks = ~500ms
volatile uint8_t t2_ticks = 0;
void onTimer2() {
  t2_ticks++;
  if (t2_ticks >= 16) {
    t2_ticks = 0;
    GPIO.toggle(PORTB, PB7);    // Blink LED on PB7 every ~500ms
  }
}

int main() {
  // --- UART ---
  UART.init(9600, true);                   // true = enable RX interrupt
  UART.attachRxInterrupt(onUARTReceive);

  // --- GPIO ---
  GPIO.setDirection(DDRA,  ALL, INPUT);    // PORTA = ADC inputs
  GPIO.setDirection(DDRB,  ALL, OUTPUT);   // PORTB = 7-seg + LED

  // --- ADC (continuous, interrupt-driven) ---
  ADC.init(ADC_REF_AVCC, ADC_PRESCALER_64, true);
  ADC.attachInterrupt(onADCReady);
  ADC.startContinuous(ADC_CHANNEL_0);

  
  // --- Timer 0: 1ms tick (prescaler 64, OCR=124) ---
  Timer.initTimer0(124, TIMER0_PRESCALER_64);
  Timer.attachTimer0(onTimer0);

  // --- Timer 1: 1 second (prescaler 256, OCR=31249) ---
  Timer.initTimer1(31249, TIMER1_PRESCALER_256);
  Timer.attachTimer1(onTimer1);

  // --- Timer 2: ~31ms tick (prescaler 1024, OCR=243) ---
  Timer.initTimer2(243, TIMER2_PRESCALER_1024);
  Timer.attachTimer2(onTimer2);

  // --- Enable all hardware interrupts ---
  sei();

  UART.print("System ready!\r\n");

  while (true) {
    // Everything runs in the background via interrupts!
    // Main loop is free to do application logic here.
  }

  return 0;
}

