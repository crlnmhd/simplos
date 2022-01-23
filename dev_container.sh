#!/usr/bin/env bash
set -e

extra_params=''

print_usage() {
  printf "Usage: --tty /dev/[TTY_DEVICE]"
}

while getopts 't:' flag; do
  case "${flag}" in
    v) extra_params="--device ${OPTARG} --env SERIAL=${OPTARG}" ;;
    *) print_usag
      exit 1 ;;
  esac
done

DOCKER="podman"

${DOCKER} run \
  -it \
  --rm \
  -v "$(pwd)"/playground:/simplos:Z \
  ${extra_params} \
  --workdir /simplos \
  --user "$(id -u):$(id -g)" \
  avr_docker:latest \
  /bin/bash

