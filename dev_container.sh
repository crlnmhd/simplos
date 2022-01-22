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

DOCKER=podman

echo "hi"

${DOCKER} run \
  -it \
  --mount type=bind,source="$(pwd)/playground",target=/simplos,readonly \
  ${extra_params} \
  avr_docker:latest \
  /bin/bash

