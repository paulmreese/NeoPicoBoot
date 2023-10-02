add_executable(onboard_cytron
        onboard_cytron.cpp
        )
target_link_libraries(onboard_cytron pico_stdlib pico_neopixel)

# create map/bin/hex file etc.
pico_add_extra_outputs(onboard_cytron)
