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
PREV_TASK_SP_LOCATION := 0x8020FA
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

# Enable verbose output
DEFINES += -DVERBOSE_OUTPUT

# Enable debug ouput (disabeling this also disables verbose output)
DEFINES += -DDEBUG_OUTPUT

DEFINES += -DINITIAL_STACK_START=$(INITIAL_STACK_START)

# Count the time spent in interups as oposed to outside. Incurres some
# overhead. Print to uart using PRINT_CS_TIMING_DATA()
# DEFINES += -DSW_TIME_MEASSREMENTS

# Use priority based scheduling.
DEFINES += -DPRIORITY_SCHEDULING

########## End of define configurations ###############
SHELL := /usr/bin/env bash


SRC := src
OBJ := build
DEBUG_BUILD := tests
LOG_DIR := log

UNIT_TESTS_SRC := src/tests
TEST_SOURCES := $(wildcard $(UNIT_TESTS_SRC)/*.c)
UNIT_TEST_OBJECTS := $(patsubst $(UNIT_TESTS_SRC)/%.c, $(DEBUG_BUILD)/%.unit_test.o, $(TEST_SOURCES))

ALL_SOURCES := $(wildcard $(SRC)/*.c $(SRC)/hal/*.c)
MAIN_FILE := $(SRC)/main.c

# Needed due to https://gcc.gnu.org/bugzilla/show_bug.cgi?id=48026
ALWAYS_OPTIMIZED_FILES := $(SRC)/hal/serial.c

SOURCES := $(filter-out $(MAIN_FILE) $(ALWAYS_OPTIMIZED_FILES), $(ALL_SOURCES))

OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
ALWAYS_OPTIMIZED_OBJECTS:= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(ALWAYS_OPTIMIZED_FILES))
TEST_OBJECTS := $(patsubst $(SRC)/%.c, $(DEBUG_BUILD)/%.o, $(SOURCES))
ALWAYS_OPTIMIZED_TEST_OBJECTS := $(patsubst $(SRC)/%.c, $(DEBUG_BUILD)/%.o, $(ALWAYS_OPTIMIZED_FILES))

ASM_SRC:= src/asm
ASM_SOURCES := $(wildcard $(ASM_SRC)/*.s)
ASM_OBJECTS := $(patsubst $(ASM_SRC)/%.s, $(OBJ)/asm/%.o, $(ASM_SOURCES))

ALL_OBJECTS := $(OBJECTS)
ALL_OBJECTS += $(ALWAYS_OPTIMIZED_OBJECTS)
ALL_OBJECTS += $(ASM_OBJECTS)

SIMPLOS_EXECUTABLE := $(OBJ)/simplos.out
UNIT_TESTS := $(DEBUG_BUILD)/unit_tests.out
LATEST_EXECUTABLE_SOFTLINK := latest_executable
UNIT_TEST_OUTPUT_FILE := unit_test_output.log

.DEFAULT_GOAL := build
CC := avr-g++
AVRINC := /usr/avr/include
SIMAVR := simavr
AVR_GDB := avr-gdb
SIMAVR_DEBUG := $(SIMAVR_DEBUG)

BUILD_OPT_LEVEL := -O0
DEBUG_OPT_LEVEL := -O0
ALWAYS_OPTIMIZED_OPT_LEVEL := -Os

UNIT_TEST_FLAGS :=	-g $\
										-DMOCK_HAL $\
										-Wno-unused-parameter

CFLAGS := -std=gnu++20 $\
					-Werror $\
					-Wall $\
					-pedantic $\
					-Wextra $\
					-Wsuggest-attribute=const $\
					-Wshadow $\
					-Wpointer-arith $\
					-Wswitch-enum $\
					-Wconversion $\
					-Wstrict-overflow $\
					-Wno-aggressive-loop-optimizations $\
					-Wno-volatile $\
					-Wno-missing-field-initializers $\
					-fno-omit-frame-pointer $\
					-fno-common $\
					-fshort-enums $\
					-fno-pie $\
					-no-pie $\
					$(DEFINES) $\
					-mmcu=$(uP) $\
					-I$(AVRINC) $\
					-I/include/ $\

CLANGD_FLAGS := -ferror-limit=0 -Wno-language-extension-token

DATA_SECTIONS := 	-Wl,--section-start=.task_sp_location=$(TASK_SP_LOCATION) $\
									-Wl,--section-start=.next_task_sp_location=$(NEXT_TASK_SP_LOCATION) $\
									-Wl,--section-start=.prev_task_sp_location=$(PREV_TASK_SP_LOCATION) $\
									-Wl,--section-start=.scheduler_task_sp_location=$(SCHEDULER_TASK_SP_LOCATION) $\
									-Wl,--section-start=.kernel_data_location=$(KERNEL_DATA_LOCATION)

# Data sections that are not suitable for unit testing.
NON_UNIT_TEST_DATA_SECIONS := -Wl,--section-start=.data=$(DATA_SECTION_START)

FRAMEWORK := wiring

.PHONY: %.compdb_entry compile_commands dir all all_test log

# Borrowed from https://gist.github.com/JayKickliter/f4e1945abe1d3bbbe3263640a3669e3c.
%.compdb_entry: %.c
	@echo "    {"                                                           		>  $@
	@echo "        \"command\": \"$(CC) $(CFLAGS) $(CLANGD_FLAGS) $(DATA_SECTIONS) -c $<\"," 	>> $@
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

all: $(ALL_OBJECTS) build/main.o $(ALWAYS_OPTIMZED)
	$(CC) $(CFLAGS) $(BUILD_OPT_LEVEL) $(DATA_SECTIONS) $(NON_UNIT_TEST_DATA_SECIONS) $^ -o $(SIMPLOS_EXECUTABLE)
	avr-size -A -x $(SIMPLOS_EXECUTABLE)

all_test: $(TEST_OBJECTS) $(ALWAYS_OPTIMIZED_TEST_OBJECTS) $(UNIT_TEST_OBJECTS) $(ASM_OBJECTS)
	$(CC) $(CFLAGS) $(DEBUG_OPT_LEVEL) $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) $^ -o $(UNIT_TESTS)
	avr-size -A -x $(UNIT_TESTS)

$(OBJ)/%.o: $(SRC)/%.c
	set +e; 																																		\
	if echo $(ALWAYS_OPTIMIZED_FILES) | tr " " "\n" | grep -F -x -q "$<"; then 	\
		opt_flag=$(ALWAYS_OPTIMIZED_OPT_LEVEL);																		\
  else 																																				\
		opt_flag=$(BUILD_OPT_LEVEL); 																							\
	fi;                                                                      		\
	echo "For file: " $< "This is the opt level: " $${opt_flag}; 								\
	$(CC) $(CFLAGS) $${opt_flag} $(DATA_SECTIONS) $(NON_UNIT_TEST_DATA_SECIONS) -I$(SRC) -c -g $< -o $@

$(DEBUG_BUILD)/%.o: $(SRC)/%.c
	if echo $(ALWAYS_OPTIMIZED_FILES) | tr " " "\n" | grep -F -x -q "$<"; then 	\
		opt_flag="-Os";																														\
  else 																																				\
		opt_flag=$(DEBUG_OPT_LEVEL); 																							\
	fi ;                                                                     		\
	$(CC) $(CFLAGS) $${opt_flag} $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) $(NON_UNIT_TEST_DATA_SECIONS) -I$(SRC) -c -g $< -o $@

$(DEBUG_BUILD)/%.unit_test.o: $(UNIT_TESTS_SRC)/%.c
	if echo $(ALWAYS_OPTIMIZED_FILES) | tr " " "\n" | grep -F -x -q "$<"; then 	\
		opt_flag="-Os";																														\
  else 																																				\
		opt_flag=$(DEBUG_OPT_LEVEL); 																							\
	fi ;                                                                     		\
	$(CC) $(CFLAGS) $${opt_flag} $(UNIT_TEST_FLAGS) $(DATA_SECTIONS) -I$(SRC) -I$(UNIT_TESTS_SRC) -c -g $< -o $@

$(OBJ)/asm/%.o: $(ASM_SRC)/%.s
	$(CC) $(CFLAGS) -I$(ASM_SRC) -c -g $< -o $@

install-git-hooks:
	@mkdir -p ../githooks
	@git config core.hooksPath githooks # from project root
	@echo "Success"

flash:
	avr-size --mcu=$(uP) -C -x $(LATEST_EXECUTABLE_SOFTLINK)
	avr-objcopy -O ihex --strip-debug -R .eeprom $(LATEST_EXECUTABLE_SOFTLINK) build/out.hex
	avrdude -c $(FRAMEWORK) \
		-p $(uP) \
		-D -P $(SERIAL) \
		-b $(BAUD) \
		-U flash:w:build/out.hex

clean:
	@echo "cleaning..."
	@if [ -d build ]; then 	\
		rm -r build;					\
	fi
	@if [ -d tests ]; then 	\
		rm -r tests;					\
	fi
	@rm -f src/*.compbd_entry
	@rm -f $(UNIT_TEST_OUTPUT_FILE)

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

run_unit_tests: build_tests
	$(SIMAVR) -v -v -v -m $(uP) $(LATEST_EXECUTABLE_SOFTLINK) 2> >(tee $(UNIT_TEST_OUTPUT_FILE) >&2)
	@sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2};?)?)?[mGK]//g" -i $(UNIT_TEST_OUTPUT_FILE)
	@grep "All tests passed" $(UNIT_TEST_OUTPUT_FILE) > /dev/null


debug_simulator:
	gdb -ex "directory $(SIMAVR_SRC)"  --args $(SIMAVR_DEBUG) -v -v -v -g -m $(uP) $(SIMPLOS_EXECUTABLE)

