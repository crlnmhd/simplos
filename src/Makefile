# Use Docker container for compiling
# https://github.com/lpodkalicki/avr-toolchain-docker

CC=avr-gcc
BAUD=115200
CFLAGS= -Wall -pedantic -Wextra -fshort-enums -mmcu=atmega2560 -DF_CPU=16000000 -DBAUD=$(BAUD)
EXTRA_OPT_FLAGS=-Os
DEBUG_FLAGS=-g
SRCS=*.c

compile: $(SRCS)
	$(CC) $(CFLAGS)$^ -o out.o
	rm out_dbg.o

debug_compile:$(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_OPT_FLAGS) $(DEBUG_FLAGS) $^ -o out_dbg.o
	rm out.o


debug_asm: simplos.c
	$(CC) -S $(CFLAGS)  $^ -o out.s

info:
	$(CC) --help=warnings

clean:
	rm *.o
	rm *.hex
