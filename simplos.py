#!/usr/bin/env python3


# NOT WORKING. The simulator does not appear to be printing to stdout.
#  Could possibly be handled by a separate thread?

import argparse
import subprocess

from pathlib import Path


def get_args():
    parser = argparse.ArgumentParser(description='Simplos - A (too) simple OS.')
    parser.add_argument('--baud', type=int, nargs=1, required=True,
                        help='BAUD rate')
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument('--build-docker', action='store_true',
                       help='Rebuild docker container.')
    group.add_argument('-f', '--flash', action='store_true',
                       help='Flash simplos to an Arduino Mega board')
    group.add_argument('-b', '--build', action='store_true',
                       help='Build simplos without debug info')
    group.add_argument('-s', '--simulate', action='store_true',
                       help='Run simplos in a simulator.')
    group.add_argument('-d', '--debug', action='store_true',
                       help='Debug simplos in GDB. Use in conjuncture with --simulate.')
    group.add_argument('-c', '--clean', action='store_true',
                       help='Remove build files.')

    args = parser.parse_args()
    return args


def main(baud, build_docker, flash, build, simulate, debug, clean):
    commands = []

    src_dir_path = Path('src/')
    avr_docker = (f'sudo docker run -it --rm --mount type=bind,source={src_dir_path.resolve()},target=/build'
                  ' avr_docker')

    build_commands = [avr_docker + f' scons baud={baud}']

    if build_docker:
        commands += ['sudo docker build --rm -t avr_docker .']
    if flash:
        commands += [avr_docker] + [f' scons baud={baud}']
        commands += ['avr-size -C src/simplos.out']
        commands += ['avr-objcopy -O ihex -R .eeprom src/simplos.o out.hex']
        commands += [('avrdude -C /usr/share/arduino/hardware/tools/avrdude.conf'
                      ' -c wiring'
                      ' -p ATMEGA2560'
                      ' -D -P /dev/ttyUSB0'
                      f' -b {baud}'
                      ' -U flash:w:out.hex')]
    if build:
        commands += build_commands
    if simulate:
        commands += build_commands
        commands += ['simavr -g -m atmega2560 src/simplos.out']
    if debug:
        commands += ['avr-gdb src/simplos.out -ex "target remote :1234" -ex "directory src/"']
    if clean:
        commands = ['rm -f src/*.o src/*.hex']

    formatted_commands = [command.split() for command in commands]

    # Run command and raise error if return code is not 0.
    for command_series in formatted_commands:
        terminal_emulator = ['/usr/bin/alacritty', '-e']
        command_series = terminal_emulator + command_series
        print('trying to execute:', command_series)

        term_command = ' '.join(command_series)
        proc = subprocess.run(command_series, stdout=subprocess.PIPE,
                                stderr=subprocess.PIPE, text=True)
        

if __name__ == '__main__':
    argv = get_args()

    main(baud=argv.baud[0],
         build_docker=argv.build_docker,
         flash=argv.flash,
         build=argv.build,
         simulate=argv.simulate,
         debug=argv.debug,
         clean=argv.clean)
