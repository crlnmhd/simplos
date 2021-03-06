FROM fedora:33

MAINTAINER Carl Nimhed carl.nimhed@icloud.com

RUN dnf install -y avr-gcc make avr-libc

WORKDIR build


# Install AVR toolchain
#ARG AVR_TOOLCHAIN_TARBALL=avr8-gnu-toolchain-3.6.2.1759-linux.any.x86_64.tar.gz
#ARG AVR_TOOLCHAIN_PATH=${TOOLS_PATH}/avr-toolchain
#COPY ${TARBALLS_PATH}/${AVR_TOOLCHAIN_TARBALL} .
#RUN tar -xvf ${AVR_TOOLCHAIN_TARBALL} \
#        && mv `tar -tf ${AVR_TOOLCHAIN_TARBALL} | head -1` ${AVR_TOOLCHAIN_PATH} \
#        && rm ${AVR_TOOLCHAIN_TARBALL}


