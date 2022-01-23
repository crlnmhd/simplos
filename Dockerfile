# Build avr-gdb from source.
# Requires an empty directory to be bind mounted to /out_dir.

# Known to work. Some problems from 20.10 and later, especially avr-gdb.
FROM ubuntu:20.04

# source directory and output directory. The latter should be bind-mounted to the host.
# RUN mkdir /simplos
RUN apt-get update -y && apt-get install gcc-avr gdb-avr simavr avr-libc avrdude -y

ARG USER_ID
ARG GROUP_ID

# Create a regular user to minimise security exposure.
#RUN groupadd -g ${GROUP_ID} duser && \
#    useradd -l -u ${USER_ID} -g duser duser
RUN addgroup --gid $GROUP_ID user
RUN adduser --disabled-password --gecos '' --uid $USER_ID --gid $GROUP_ID user
USER user
