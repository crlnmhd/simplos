#!/usr/bin/env bash

DOCKER=podman

${DOCKER} build \
  --build-arg USER_ID=$(id -u) \
  --build-arg GROUP_ID=$(id -g) \
  --rm -t \
  avr_docker .
