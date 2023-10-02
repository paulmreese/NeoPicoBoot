#!/bin/bash
# Paul M Reese 2023
test -d build || mkdir build
test -d PicoWNeoPixelServer/tools/SimpleFSBuilder/build || mkdir -p PicoWNeoPixelServer/tools/SimpleFSBuilder/build
test -d PicoWNeoPixelServer/example/build || mkdir -p PicoWNeoPixelServer/example/build
test -f PicoWNeoPixelServer/tools/SimpleFSBuilder/buildCMakeCache.txt || cmake -S PicoWNeoPixelServer/tools/SimpleFSBuilder -B PicoWNeoPixelServer/tools/SimpleFSBuilder/build
test -e PicoWNeoPixelServer/tools/SimpleFSBuilder/SimpleFSBuilder || make -C PicoWNeoPixelServer/tools/SimpleFSBuilder/build || (echo "Filesystem failed to build. Try running the 'build-filesystem.sh' script" && exit 1)

test -d PicoWNeoPixelServer/pico-sdk || git clone --recursive https://github.com/raspberrypi/pico-sdk
test -d PicoWNeoPixelServer/pico-sdk/FreeRTOS || git clone --recursive --branch smp https://github.com/FreeRTOS/FreeRTOS-Kernel PicoWNeoPixelServer/pico-sdk/FreeRTOS
grep -e ip4_secondary_ip_address PicoWNeoPixelServer/pico-sdk/lib/lwip/src/core/ipv4/ip4.c || patch -p1 -d PicoWNeoPixelServer/pico-sdk/lib/lwip < PicoWNeoPixelServer/lwip_patch/lwip.patch || (echo "Failed to apply patch" && exit 1)

test -f src/ipl.h || ./process_ipl.py src/iplboot.dol src/ipl.h || (echo "IPL boot failed to process. Have you downloaded an 'iplboot.dol' file and copied it into the 'src' folder?" && exit 1)

echo "If the filesystem required (re)building, some values set as compiler options in CMakeLists.txt may be ignored. Please verify your settings, most importantly your SSID and Password"
cmake -S . -B build -DPICO_SDK_PATH=`pwd`/PicoWNeoPixelServer/pico-sdk -DPICO_GCC_TRIPLE=arm-none-eabi
make -C build || (echo "Failed to build." && exit 1)
