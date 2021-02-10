CC=avr-gcc
BAUD=115200
CFLAGS= -Wall -pedantic -Os -mmcu=atmega2560 -DF_CPU=16000000 -DBAUD=$(BAUD)

SRCS=*.c

# Lazy, but it works...
upload: compile
	avr-size -C out.o
	avr-objcopy -O ihex -R .eeprom out.o out.hex
	avrdude -C /usr/share/arduino/hardware/tools/avrdude.conf  -c wiring -p ATMEGA2560 -D -P /dev/ttyUSB0 -b $(BAUD) -U flash:w:out.hex	

debug: compile


compile: 	 $(SRCS)
		$(CC) $(CFLAGS) $^ -o out.o

clean:
	rm *.o
	rm *.hex