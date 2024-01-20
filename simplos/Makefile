# AVR-c makefile
# Looks for source and header files in src/ and places .o files in build


############### Configurations ######################
uP := atmega2560 #  (e.g. atmega328p, atmega2560)
BAUD := 115200
CPU_FREQ := 16000000UL
SERIAL := /dev/ttyUSB0

INITIAL_STACK_START := 0x1FFF

TASK_SP_LOCATION := 0x802100
SCHEDULER_TASK_SP_LOCATION := 0x8020FE
NEXT_TASK_SP_LOCATION := 0x8020FC
KERNEL_DATA_LOCATION := 0x802102
DATA_SECTION_START := 0x802000

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

# Enable debug ouput
DEFINES += -DDEBUG_OUTPUT

DEFINES += -DINITIAL_STACK_START=$(INITIAL_STACK_START)

# Count the time spent in interups as oposed to outside. Incurres some
# overhead. Print to uart using PRINT_CS_TIMING_DATA()
# DEFINES += -DSW_TIME_MEASSREMENTS

# Use priority based scheduling.
DEFINES += -DPRIORITY_SCHEDULING

########## End of define configurations ###############

SRC := src
OBJ := build
DEBUG_BUILD := tests
LOG_DIR := log

UNIT_TESTS_SRC := src/tests
TEST_SOURCES := $(wildcard $(UNIT_TESTS_SRC)/*.c)
UNIT_TEST_OBJECTS := $(patsubst $(UNIT_TESTS_SRC)/%.c, $(DEBUG_BUILD)/%.unit_test.o, $(TEST_SOURCES))

ALL_SOURCES := $(wildcard $(SRC)/*.c $(SRC)/hal/*.c)
MAIN_FILE := $(SRC)/main.c
SOURCES := $(filter-out $(MAIN_FILE), $(ALL_SOURCES))

OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
TEST_OBJECTS := $(patsubst $(SRC)/%.c, $(DEBUG_BUILD)/%.o, $(SOURCES))

ASM_SRC:= src/asm
ASM_SOURCES := $(wildcard $(ASM_SRC)/*.s)
ASM_OBJECTS := $(patsubst $(ASM_SRC)/%.s, $(OBJ)/asm/%.o, $(ASM_SOURCES))

ALL_OBJECTS := $(OBJECTS)
ALL_OBJECTS += $(ASM_OBJECTS)

SIMPLOS_EXECUTABLE := $(OBJ)/simplos.out
UNIT_TESTS := $(DEBUG_BUILD)/unit_tests.out
LATEST_EXECUTABLE_SOFTLINK := latest_executable

.DEFAULT_GOAL := build
CC := avr-gcc
AVRINC := /usr/avr/include
SIMAVR := simavr
AVR_GDB := avr-gdb
SIMAVR_DEBUG := $(SIMAVR_DEBUG)

BUILD_OPT_LEVEL := -Os
DEBUG_OPT_LEVEL := -Os

UNIT_TEST_FLAGS :=	-g $\
										-DMOCK_HAL

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
					-fno-common $\
					$(DEFINES) $\
					-mmcu=$(uP) $\
					-I$(AVRINC) $\
					-I/include/ $\
					-g

DATA_SECTIONS :=	-Wl,--section-start=.data=$(DATA_SECTION_START) $\
									-Wl,--section-start=.task_sp_location=$(TASK_SP_LOCATION) $\
									-Wl,--section-start=.next_task_sp_location=$(NEXT_TASK_SP_LOCATION) $\
									-Wl,--section-start=.scheduler_task_sp_location=$(SCHEDULER_TASK_SP_LOCATION) $\
									-Wl,--section-start=.kernel_data_location=$(KERNEL_DATA_LOCATION)


FRAMEWORK := wiring

.PHONY: %.compdb_entry compile_commands dir all all_test log

# Borrowed from https://gist.github.com/JayKickliter/f4e1945abe1d3bbbe3263640a3669e3c.
%.compdb_entry: %.c
	@echo "    {"                                                           		>  $@
	@echo "        \"command\": \"$(CC)  $(CFLAGS) $(DATA_SECTIONS) -c $<\"," 	>> $@
	@echo "        \"directory\": \"$(CURDIR)\","            										>> $@
	@echo "        \"file\": \"$<\""                         										>> $@
	@echo "    },"                                           										>> $@

COMPDB_ENTRIES = $(addsuffix .compdb_entry, $(basename $(SOURCES)))
COMPDB_ENTRIES += $(addsuffix .compdb_entry, $(basename $(TEST_SOURCES)))

compile_commands: $(COMPDB_ENTRIES)
	@echo "[" > $@.tmp
	@cat $^ >> $@.tmp
	@sed '$$d' < $@.tmp > $@.json
	@echo "    }" >> $@.json
	@echo "]" >> $@.json
	@rm $@.tmp
	@rm $^


dir:
	mkdir -p $(OBJ)
	mkdir -p $(OBJ)/asm
	mkdir -p $(OBJ)/tests
	mkdir -p $(OBJ)/hal
	mkdir -p $(LOG_DIR)
	mkdir -p $(DEBUG_BUILD)
	mkdir -p $(DEBUG_BUILD)/hal

# Target to compile and generate database for clangd.
build: compile_commands dir all
	ln -s -f $(SIMPLOS_EXECUTABLE) $(LATEST_EXECUTABLE_SOFTLINK)

build_tests: compile_commands dir all_test
	ln -s -f $(UNIT_TESTS) $(LATEST_EXECUTABLE_SOFTLINK)

all: $(OBJECTS) $(ASM_OBJECTS) build/main.o
	$(CC) $(CFLAGS) $(BUILD_OPT_LEVEL) $(DATA_SECTIONS) $^ -o $(SIMPLOS_EXECUTABLE)
	avr-size -A -x $(SIMPLOS_EXECUTABLE)

all_test: $(TEST_OBJECTS) $(UNIT_TEST_OBJECTS) $(ASM_OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG_OPT_LEVEL) $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) $^ -o $(UNIT_TESTS)
	avr-size -A -x $(UNIT_TESTS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(BUILD_OPT_LEVEL) $(DATA_SECTIONS) -I$(SRC) -c -g $< -o $@

$(DEBUG_BUILD)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(DEBUG_OPT_LEVEL) $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) -I$(SRC) -I$(UNIT_TESTS_SRC) -c -g $< -o $@

$(DEBUG_BUILD)/%.unit_test.o: $(UNIT_TESTS_SRC)/%.c
	$(CC) $(CFLAGS) $(DEBUG_OPT_LEVEL) $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) -I$(SRC) -I$(UNIT_TESTS_SRC) -c -g $< -o $@

$(OBJ)/asm/%.o: $(ASM_SRC)/%.s
	$(CC) $(CFLAGS) -I$(ASM_SRC) -c -g $< -o $@


flash:
	avr-size --mcu=$(uP) -C -x $(LATEST_EXECUTABLE_SOFTLINK)
	avr-objcopy -O ihex --strip-debug -R .eeprom $(LATEST_EXECUTABLE_SOFTLINK) build/out.hex
	avrdude -c $(FRAMEWORK) \
		-p $(uP) \
		-D -P $(SERIAL) \
		-b $(BAUD) \
		-U flash:w:build/out.hex

clean:
	@if [ -d build ]; then 	\
		rm -r build;					\
	fi
	@if [ -d tests ]; then 	\
		rm -r tests;					\
	fi
	@rm -f src/*.compbd_entry
	@echo "cleaning..."

log:
	rm -f gdb.log
	$(AVR_GDB) $(LATEST_EXECUTABLE_SOFTLINK) -x  log_execution.gdb
	mv gdb.log $(LOG_DIR)/simplos_$$(date +%Y%m%d_%H%M%S).log

sim:
	$(SIMAVR) -v -v -v -g -m $(uP) $(LATEST_EXECUTABLE_SOFTLINK)

show-size:
	avr-size --mcu=$(uP) -A -x $(SIMPLOS_EXECUTABLE)

debug:
	$(AVR_GDB) $(LATEST_EXECUTABLE_SOFTLINK) -x debug_config.gdb


debug_simulator:
	gdb -ex "directory $(SIMAVR_SRC)"  --args $(SIMAVR_DEBUG) -v -v -v -g -m $(uP) $(SIMPLOS_EXECUTABLE)

