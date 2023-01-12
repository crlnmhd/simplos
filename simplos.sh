#!/usr/bin/env bash

set -eo pipefail

main(){
  assert_os_is_supported

  if asked_to_rebuild_dev_container "$@"; then
    build_dev_container
  elif asked_to_run_dev_container "$@"; then
    enter_dev_container
  else
    show_usage_help
  fi
  exit 0
}

assert_os_is_supported(){
  if ! os_supported; then
    echo "OS not supported. Exiting."
    exit 1
  fi
}

asked_to_run_dev_container(){
  if [[ $# -eq 0  || ($# -eq 1 && $1 == "dev") ]]; then
    return 0
  else
    return 1
  fi
}

asked_to_rebuild_dev_container(){
  if [[ $# -eq 1 && $1 == "build-container" ]]; then
    return 0
  else
    return 1
  fi
}

show_usage_help(){
  echo "Usage: ./simplos [dev] [build-container]"
}

os_supported() {
  if ! is_installed "selinuxenabled"; then
    echo "Expected SELinux to be available. Consider removing the :z flag to the bind mount."
    return 1
  elif ! is_installed "podman"; then
    echo "Expected a system with podman."
    echo "Consider installing podman or use docker with approriate uid/gid and permission"
    return 2
  else
    return 0
  fi
}

is_installed() {
  if which "$1" > /dev/null; then # $1 installed
    return 0
  else
    return 1
  fi
}

build_dev_container() {
  echo "building dev container"
  podman build --rm -t avr_docker .
}

enter_dev_container() {
  echo "entering dev container"

  local source_directory
  source_directory="$(pwd)/simplos"

  local mount_destination
  mount_destination="/simplos"

  podman run -it --rm -v "${source_directory}:${mount_destination}":Z avr_docker bash
}

main "$@"
