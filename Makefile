# AVR-c makefile
# Looks for source and header files in src/ and places .o files in build


############### Configurations ######################
uP := atmega2560 #  (e.g. atmega328p, atmega2560)
BAUD := 115200
CPU_FREQ := 16000000UL
SERIAL := /dev/ttyUSB0

# Configure behavior though defines
DEFINES :=
# Static scheduling
DEFINES += -DUSE_STATIC

# Enables optimization. Use only for non threadsafe components.
DEFINES += -DNO_MT='__attribute__((optimize("Os")))'

# Enable verbose output
# DEFINES += -DVERBOSE_OUTPUT

# Use attached device on  22-26 to output current task as status during context
# switch. Poll this over time on a separate device to gain knowledge of system
# performance and how the ratio of time spent in context switches. Very small
# overhead.
DEINES += -DHW_TIME_MEASSUREMENTS

# Measure time only in SW
# DEFINES += -DSW_TIME_MEASSREMENTS

# Enable debug ouput
DEFINES += -DDEBUG_OUTPUT

#enable tests
DEFINES += -DRUN_TESTS

# Count the time spent in interups as oposed to outside. Incurres some
# overhead. Print to uart using PRINT_CS_TIMING_DATA()
# DEFINES += -DSW_TIME_MEASSREMENTS

# Use priority based scheduling.
# DEINFES += -DPRIORITY_SCHEDULING

########## End of define configurations ###############

SRC := src
OBJ := build

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

.DEFAULT_GOAL := build
CC := avr-gcc
AVRINC :=/usr/avr/include
SIMAVR := simavr
CFLAGS := -Wall -pedantic -Wextra -Wstrict-prototypes -fshort-enums -std=gnu17 $(DEFINES) -mmcu=$(uP) -Wno-unknown-attributes -I$(AVRINC) -I/include/ -DF_CPU=$(CPU_FREQ) -DBAUD=$(BAUD) -g
FRAMEWORK := wiring

AVR_GDB := /home/cgn/prog/external/avr-gdb/avr-gdb-8.3/bin/avr-gdb
.PHONY: %.compdb_entry compile_commands.json dir all

# Borrowed from https://gist.github.com/JayKickliter/f4e1945abe1d3bbbe3263640a3669e3c.
%.compdb_entry: %.c
	@echo "    {" > $@
	@echo "        \"command\": \"$(CC)  $(CFLAGS) $(CPPFLAGS) -c $<\","   >> $@
	@echo "        \"directory\": \"$(CURDIR)\","               >> $@
	@echo "        \"file\": \"$<\""                    >> $@
	@echo "    },"                              >> $@

COMPDB_ENTRIES = $(addsuffix .compdb_entry, $(basename $(SOURCES)))

compile_commands.json: $(COMPDB_ENTRIES)
	@echo "[" > $@.tmp
	@cat $^ >> $@.tmp
	@sed '$$d' < $@.tmp > $@
	@echo "    }" >> $@
	@echo "]" >> $@
	@rm $@.tmp
	@rm $^


# Create build directory only if it doesn't exits.
dir:
	mkdir -p build

# Target to compile and generate database for clangd.
build: dir all compile_commands.json

all: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o build/simplos.out
	avr-size --mcu=$(uP) -C build/simplos.out

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c -g $< -o $@

flash: build
	avr-size --mcu=$(uP) -C build/simplos.out
	avr-objcopy -O ihex --strip-debug -R .eeprom build/simplos.out build/out.hex
	avrdude -c $(FRAMEWORK) \
		-p $(uP) \
		-D -P $(SERIAL) \
		-b $(BAUD) \
		-U flash:w:build/out.hex

clean:
	@if [[ -e build ]]; then \
		rm -r build;						\
		fi
	@rm -f src/*.compbd_entry
	@echo "cleaning..."

sim:
	$(SIMAVR) -v -v -v -g -m $(uP) build/simplos.out

debug:
	$(AVR_GDB) build/simplos.out -ex "target remote :1234" -ex "directory src/"

rebuild_container:
	$(DOCKER) build --rm -t avr_docker .

build_gdb:
	$(DOCKER) run -it --rm --mount type=bind,source=$(PWD)/gdb_build,target=/out_dir avr_docker


#scons)
#  sudo docker run -it --rm --mount type=bind,source=$PWD/src,target=/build avr_docker scons baud=$BAUD debug
#  avr-size --mcu=$uP -C src/simplos.out
#  ;;
#build)
#  sudo docker run -it --rm --mount type=bind,source=$PWD/src,target=/build avr_docker scons baud=$BAUD
#  avr-size --mcu=$uP -C src/simplos.out
#  ;;
#
#flash)
#  sudo docker run -it --rm --mount type=bind,source=$PWD/src,target=/build avr_docker scons baud=$BAUD
#  BAUD=115200
#  avr-size --mcu=$uP -C src/simplos.out
#  avr-objcopy -O ihex -R .eeprom src/simplos.out out.hex
#  avrdude -C /usr/share/arduino/hardware/tools/avrdude.conf -c wiring \
	#    -p $uP \
	#    -D -P /dev/ttyUSB0 \
	#    -b $BAUD \
	#    -U flash:w:out.hex \
	#    /
