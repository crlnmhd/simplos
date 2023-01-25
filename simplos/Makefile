# AVR-c makefile
# Looks for source and header files in src/ and places .o files in build


############### Configurations ######################
uP := atmega2560 #  (e.g. atmega328p, atmega2560)
BAUD := 115200
CPU_FREQ := 16000000UL
SERIAL := /dev/ttyUSB0

INITIAL_STACK_START := 0x1FFF

TASK_SP_LOCATION := 0x802100
KERNEL_DATA_LOCATION := 0x802102
DATA_SECTION_START := 0x802000

# Configure behavior though defines
DEFINES :=
# Static scheduling
# DEFINES += -DUSE_STATIC

# Frequency
DEFINES += -DF_CPU=$(CPU_FREQ)

# Baud rate for serial
DEFINES += -DBAUD=$(BAUD)

# Enables optimization. Use only for non threadsafe components.
# DEFINES += -DNO_MT='__attribute__((optimize("Os")))'
DEFINES += -DNO_MT='' # '__attribute__((optimize("Os")))'

# Enable verbose output
DEFINES += -DVERBOSE_OUTPUT

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

DEFINES += -DSTACK_HIGH=$(STACK_HIGH)
DEFINES += -DINITIAL_STACK_START=$(INITIAL_STACK_START)

# Count the time spent in interups as oposed to outside. Incurres some
# overhead. Print to uart using PRINT_CS_TIMING_DATA()
# DEFINES += -DSW_TIME_MEASSREMENTS

# Use priority based scheduling.
DEFINES += -DPRIORITY_SCHEDULING

########## End of define configurations ###############

SRC := src
OBJ := build

ASM_SRC:= src/asm

SOURCES := $(wildcard $(SRC)/*.c $(SRC)/*/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

ASM_SOURCES := $(wildcard $(ASM_SRC)/*.s)
ASM_OBJECTS := $(patsubst $(ASM_SRC)/%.s, $(OBJ)/asm/%.o, $(ASM_SOURCES))

ALL_OBJECTS := $(OBJECTS)
ALL_OBJECTS += $($ASM_OBJECTS)

$(info ${ALL_OBJECTS})

.DEFAULT_GOAL := build
CC := avr-gcc
AVRINC := /usr/avr/include
SIMAVR := simavr
AVR_GDB := avr-gdb
SIMAVR_DEBUG := $(SIMAVR_DEBUG)
CFLAGS := -Wall $\
					-pedantic $\
					-Wextra $\
					-Wstrict-prototypes $\
					-fshort-enums $\
					-fno-pie $\
					-no-pie $\
					-std=gnu17 $\
					-Wshadow $\
					-Wpointer-arith $\
					-Wswitch-enum $\
					-Wconversion $\
					-Wstrict-overflow $\
					-Wno-aggressive-loop-optimizations $\
					-Wno-unknown-attributes $\
					-fno-omit-frame-pointer $\
					$(DEFINES) $\
					-mmcu=$(uP) $\
					-I$(AVRINC) $\
					-I/include/ $\
					-I$(TESTS_DIR) $\
					-g $\
					-Wl,--section-start=.data=$(DATA_SECTION_START) $\
 					-Wl,--section-start=.task_sp_location=$(TASK_SP_LOCATION) $\
 					-Wl,--section-start=.kernel_data_location=$(KERNEL_DATA_LOCATION) $\
					-Os

FRAMEWORK := wiring

.PHONY: %.compdb_entry compile_commands.json dir all

# Borrowed from https://gist.github.com/JayKickliter/f4e1945abe1d3bbbe3263640a3669e3c.
%.compdb_entry: %.c
	@echo "    {"                                             > $@
	@echo "        \"command\": \"$(CC)  $(CFLAGS) -c $<\"," >> $@
	@echo "        \"directory\": \"$(CURDIR)\","            >> $@
	@echo "        \"file\": \"$<\""                         >> $@
	@echo "    },"                                           >> $@

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
	mkdir -p $(OBJ)
	mkdir -p $(OBJ)/asm
	mkdir -p $(OBJ)/tests

# Target to compile and generate database for clangd.
build: compile_commands.json dir all

all: $(OBJECTS) $(ASM_OBJECTS)
	$(CC) $(CFLAGS) $^ -o build/simplos.out
	avr-size -A -x build/simplos.out

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c -g $< -o $@

$(OBJ)/asm/%.o: $(ASM_SRC)/%.s
	$(CC) $(CFLAGS) -I$(ASM_SRC) -c -g $< -o $@


flash: build
	avr-size --mcu=$(uP) -A -x build/simplos.out
	avr-objcopy -O ihex --strip-debug -R .eeprom build/simplos.out build/out.hex
	avrdude -c $(FRAMEWORK) \
		-p $(uP) \
		-D -P $(SERIAL) \
		-b $(BAUD) \
		-U flash:w:build/out.hex

clean:
	@if [ -d build ]; then 	\
		rm -r build;					\
	fi
	@rm -f src/*.compbd_entry
	@echo "cleaning..."

sim:
	$(SIMAVR) -v -v -v -g -m $(uP) build/simplos.out

show-size:
	avr-size --mcu=$(uP) -A -x build/simplos.out

debug:
	$(AVR_GDB) build/simplos.out -x debug_config.gdb

debug_siulator:
	gdb -ex "directory $(SIMAVR_SRC)"  --args $(SIMAVR_DEBUG) -v -v -v -g -m $(uP) build/simplos.out


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
