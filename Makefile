# # Use Docker container for compiling
# # https://github.com/lpodkalicki/avr-toolchain-docker

# CC=sudo docker run  -it --rm --mount type=bind,source=$(PWD)/src,target=/build avr_docker avr-gcc
# BAUD=115200
# CFLAGS= -v -Wall -pedantic -fshort-enums -Os -mmcu=atmega2560 -DF_CPU=16000000 -DBAUD=$(BAUD)
# EXTRA_OPT_FLAGS=-flto
# DEBUG_FLAGS=-g
# SRCS=build/*.c

# # Lazy, but it works...
# upload: compile
# 	avr-size -C out.o
# 	avr-objcopy -O ihex -R .eeprom out.o out.hex
# 	avrdude -C /usr/share/arduino/hardware/tools/avrdude.conf  -c wiring -p ATMEGA2560 -D -P /dev/ttyUSB0 -b $(BAUD) -U flash:w:out.hex	

# debug_server: $(SRCS)
# 	# Sudo for docker
# 	sudo $(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o out.o
# 	avr-size -C out.o
# 	simavr -g -m atmega2560 out.o
	
# debug_gdb:
# 	avr-gdb out.o -ex "target remote :1234"

# debug_asm: simplos.c
# 	sudo $(CC) -S $(CFLAGS)  $^ -o out.s

# container:
# 	sudo docker run  -it --rm --mount type=bind,source=$(PWD)/src,target=/build avr_docker 

# clean:
# 	rm *.o
# 	rm *.hex


# # CC=sudo docker run  -it --rm --privileged -v $(pwd):/build avr_docker avr-gcc
# # BAUD=115200
# # CFLAGS= -v -Wall -pedantic -fshort-enums -Os -mmcu=atmega2560 -DF_CPU=16000000 -DBAUD=$(BAUD)
# # EXTRA_OPT_FLAGS=-flto
# # DEBUG_FLAGS=-g
# # SRCS=*.c

# # # Lazy, but it works...
# # upload: compile
# # 	avr-size -C out.o
# # 	avr-objcopy -O ihex -R .eeprom out.o out.hex
# # 	avrdude -C /usr/share/arduino/hardware/tools/avrdude.conf  -c wiring -p ATMEGA2560 -D -P /dev/ttyUSB0 -b $(BAUD) -U flash:w:out.hex	

# # debug_server: $(SRCS)
# # 	# Sudo for docker
# # 	sudo $(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o out.o
# # 	avr-size -C out.o
# # 	simavr -g -m atmega2560 out.o
	
# # debug_gdb:
# # 	avr-gdb out.o -ex "target remote :1234"

# # compile: $(SRCS)
# # 	@ $(CC) $(CFLAGS) $(EXTRA_OPT_FLAGS) $^ -o out.o

# # debug_asm: simplos.c
# # 	sudo $(CC) -S $(CFLAGS)  $^ -o out.s

# # container:
# # 	sudo docker run  -it --rm --privileged -v $(pwd):/build avr_docker pwd

# # clean:
# # 	rm *.o
# # 	rm *.hex
