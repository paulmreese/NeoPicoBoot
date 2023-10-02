add_library(pico_neopixel_animations INTERFACE)

target_sources(pico_neopixel_animations INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/pico_neopixel_animations.cpp
)

# Include the Neopixel directory
add_subdirectory(pico_neopixels)

# Enable USB, UART output
pico_enable_stdio_usb(pico_neopixel_animations 1)
pico_enable_stdio_uart(pico_neopixel_animations 0)

# Include files in the current directory
target_include_directories(pico_neopixel_animations INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# Link the Project to libraries (cyw43_arch, pico_stdlib)
target_link_libraries(
        pico_neopixel_animations INTERFACE
        pico_stdlib
        pico_neopixel
        )
