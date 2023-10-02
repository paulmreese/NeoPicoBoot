add_executable(simple
        simple.cpp
        )

pico_enable_stdio_usb(simple 1)
pico_enable_stdio_uart(simple 0)

target_link_libraries(simple pico_stdlib pico_neopixel)

# create map/bin/hex file etc.
pico_add_extra_outputs(simple)
