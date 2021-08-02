import os
import scons_compiledb

uP = "atmega2560"
src_files = Glob("src/*.c")
header_files = Glob("src/*.h")

env = Environment(
    CC="avr-gcc",
    CPATH="src",
    CFLAGS=Split(
        """ -Wall
            -pedantic
            -Wextra
            -Wstrict-prototypes
            -fshort-enums
            -std=gnu17
            -mmcu=atmega2560
            -Wno-unknown-attributes
            -DF_CPU=16000000
            -DDEBUG_OUTPUT
            -flto
            -I/usr/avr/include
            -Wnonewline-eof
            -Wnolanguage-extension-token
            -Os"""
    ),
)
# Setup a compilation database with the --compiledb option
scons_compiledb.enable_with_cmdline(env)

# env.Tool("compilation_db")
# env.CompilationDatabase()
# User environment terminal may support features like colored output
# (e.g. warnings and errors)
env["ENV"]["TERM"] = os.environ["TERM"]

# Builder to format source code.
# clang_format = env.Command(
#     "target",
#     src_files + header_files,
#     'clang-format -i -style=Google $SOURCES && echo "FORMATING DONE!"',
# )

baud_rate = ARGUMENTS.get("baud", 115200)

env.Append(CFLAGS=f"-DBAUD={baud_rate}")

env.Append(LINKFLAGS="-mmcu=atmega2560")

system_cpu_count = len(os.sched_getaffinity(0))
num_cpu = int(os.environ.get("NUM_CPU", system_cpu_count))
SetOption("num_jobs", num_cpu)

debug_mode = ARGUMENTS.get("debug", None)

# Make avr path explicit to help clang-tidy
env.Append(C_INCLUDE_PATH="/usr/avr/include")

if debug_mode:
    env.Append(CFLAGS="-g")
    env.Append(LINKFLAGS="-g")

# formatted_sources =

simplos = env.Program(target="simplos.out", source=src_files)


print("BUILD_TARGETS is %s" % [str(t) for t in BUILD_TARGETS])

# env.AddPreAction(simplos, clang_format)

# simulator = env.Command(''
#     'avr-size --mcu=$uP -C src/simplos.out && simavr -g -m ' + uP + 'src/simplos.out')

# avr_gdb = '/home/cgn/prog/external/avr-gdb/avr-gdb/bin/avr-gdb'
# debug = env.Command('avr-size --mcu=$uP -C src/simplos.out && ' + avr_gdb + 'src/simplos.out -ex "target remote :1234" -ex "directory src/"')


# env.Alias('sim', [simplos, simulator])
Default(simplos)
