FROM ubuntu:20.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install gcc g++ make avrdude gdb-avr git libmpc-dev libgmp3-dev libmpfr-dev flex wget tmux libelf-dev pkg-config freeglut3-dev -y

RUN mkdir /build_tools && mkdir /build_tools/avr_gcc && mkdir /build_tools/binutils && mkdir /build_tools/bin && mkdir /simplos

ENV PREFIX=/build_tools
ENV PATH=$PATH:$PREFIX/bin


RUN echo "Downloading binutils..." && \
  cd /build_tools && \
  wget -q https://ftp.gnu.org/gnu/binutils/binutils-2.38.tar.gz && \
  echo "Uncompressing binutils..." && \
  tar zxf binutils-2.38.tar.gz -C binutils && \
  rm binutils-2.38.tar.gz && \
  cd binutils/binutils-2.38 && \
  touch -d1970-01-01 gas/doc/.dirstamp && \
  mkdir obj-avr && \
  cd obj-avr && \
  echo "building binutils..." && \
  ../configure --prefix=$PREFIX --target=avr --disable-nls && \
  make -j $(nproc) && \
  make install

RUN echo "Downloading avr-gcc..." && \
  cd /build_tools && \
  git archive --format=tgz --remote=git://gcc.gnu.org/git/gcc.git releases/gcc-8.5.0 > avr_gcc.tgz && \
  echo "Uncompressing avr-gcc..." && \
  tar zxf avr_gcc.tgz -C avr_gcc && \
  rm avr_gcc.tgz && \
  cd avr_gcc && \
  mkdir obj-avr && \
  cd obj-avr && \
  echo "Building avr-gcc..." && \
  ../configure --prefix=$PREFIX --target=avr --enable-languages=c --disable-nls --disable-libssp --with-dwarf2 && \
  make -j $(nproc) && \
  echo "installing " && \
  make install

RUN echo "Downloading avr-libc..." && \
  cd /build_tools && \
  wget -q https://download.savannah.gnu.org/releases/avr-libc/avr-libc-2.1.0.tar.bz2 && \
  echo "Uncompressing avr-libc..." && \
  tar -xf avr-libc-2.1.0.tar.bz2 && \
  rm avr-libc-2.1.0.tar.bz2 && \
  cd avr-libc-2.1.0 && \
  echo "building avr-libc..." && \
  ./configure --prefix=$PREFIX --build=`./config.guess` --host=avr --with-debug-info=dward-2 && \
  make -j $(nproc) && \
  make install

RUN echo "Downloading simavr ..." && \
  cd /build_tools && \
  wget -q https://github.com/buserror/simavr/archive/refs/tags/v1.7.tar.gz && \
  echo "Uncompressing simavr..." && \
  tar -xf v1.7.tar.gz && \
  rm v1.7.tar.gz && \
  cd simavr-1.7 && \
  echo "building simavr..." && \
  make -j $(nproc) && \
  make install RELEASE=1

WORKDIR /simplos
