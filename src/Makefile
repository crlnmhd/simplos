# Use Docker container for compiling
# https://github.com/lpodkalicki/avr-toolchain-docker

CC=avr-gcc
CFLAGS= -Wall -pedantic -Wextra -Wstrict-prototypes -fshort-enums -mmcu=atmega2560 -DF_CPU=16000000 -DBAUD=$(BAUD)
EXTRA_OPT_FLAGS=-Os
DEBUG_FLAGS=-g
SRCS=*.c

compile: $(SRCS)
	$(CC) $(CFLAGS) $(EXTRA_OPT_FLAGS) $^ -o out.o
	rm out_dbg.o

main_asm: simplos.c
	$(CC) $(CFLAGS) $(EXTRA_OPT_FLAGS) -S $^ -o simplos_asm.s

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
