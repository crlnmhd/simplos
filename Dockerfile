# Build avr-gdb from source.
# Requires an empty directory to be bind mounted to /out_dir and a directory named gdb-8.3.1 contaning the source code of gdb.

FROM ubuntu:20.04

# source directory and output directory. The latter should be bind-mounted to the host.
RUN mkdir /gdb_src&& mkdir /out_dir
RUN apt-get update -y && apt-get install make gcc g++ gawk -y

# Create a regular user to minimise security exposure.
RUN groupadd -r duser && useradd -s /bin/false -d /gdb_src -g duser duser && chown -R duser /gdb_src && chown -R duser /out_dir
USER duser
WORKDIR /gdb_src
RUN echo "Building gdb 8.3.1..."
COPY --chown=duser gdb-8.3.1 .

# out_dir should be bind mounted to the host.

  RUN ./configure --target=avr --prefix=/out_dir
CMD make -j$(nproc)
