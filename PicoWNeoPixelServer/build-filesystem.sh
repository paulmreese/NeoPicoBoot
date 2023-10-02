#!/bin/bash
test -d tools/SimpleFSBuilder/build || mkdir -p tools/SimpleFSBuilder/build
test -f tools/SimpleFSBuilder/buildCMakeCache.txt || cmake -S tools/SimpleFSBuilder -B tools/SimpleFSBuilder/build
test -e tools/SimpleFSBuilder/SimpleFSBuilder || make -C tools/SimpleFSBuilder/build || exit 1

test -d pico-sdk || git clone --recursive https://github.com/raspberrypi/pico-sdk
test -d pico-sdk/FreeRTOS || git clone --recursive --branch smp https://github.com/FreeRTOS/FreeRTOS-Kernel pico-sdk/FreeRTOS
grep -e ip4_secondary_ip_address pico-sdk/lib/lwip/src/core/ipv4/ip4.c || patch -p1 -d pico-sdk/lib/lwip < lwip_patch/lwip.patch || (echo "Failed to apply patch" && exit 1)
