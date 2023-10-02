add_executable(strandtest_wheel
        strandtest_wheel.cpp
        )
target_link_libraries(strandtest_wheel pico_stdlib pico_neopixel)

# create map/bin/hex file etc.
pico_add_extra_outputs(strandtest_wheel)
