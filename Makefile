main:
	avr-g++ -mmcu=atmega32a -Os -o main.bin main.cpp
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avrdude -c usbasp -p m32 -B 125kHz -U lfuse:w:0xe1:m -U hfuse:w:0xd9:m
	avrdude -p m32 -c usbasp -P usb -B 125kHz -U flash:w:main.hex:i
	avrdude -c usbasp -p m32 -B 125kHz -U lfuse:w:0xff:m -U hfuse:w:0xd9:m