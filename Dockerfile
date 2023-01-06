FROM ubuntu:18.04

RUN apt-get update -y && apt-get install gcc-avr gdb-avr simavr avr-libc avrdude -y

RUN mkdir /simplos
WORKDIR /simplos
