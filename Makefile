main:
	avr-g++ -mmcu=atmega32 -Os -Dnullptr=0 -o main.bin main.cpp
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xe1:m -U hfuse:w:0xd9:m
	avrdude -p m32 -c usbasp -P usb -B 8 -U flash:w:main.hex:i
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xff:m -U hfuse:w:0xd9:m

internal-1mhz:
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xe1:m -U hfuse:w:0xd9:m

internal-2mhz:
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xe2:m -U hfuse:w:0xd9:m

internal-4mhz:
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xe3:m -U hfuse:w:0xd9:m

internal-8mhz:
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xe4:m -U hfuse:w:0xd9:m

external-xtal:
	avrdude -c usbasp -p m32 -B 8 -U lfuse:w:0xff:m -U hfuse:w:0xd9:m
