add_library(PicoHTTPServer INTERFACE)

target_sources(PicoHTTPServer INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

# Add pico_neopixel_animations files
add_subdirectory(pico_neopixel_animations)

target_include_directories(
        PicoHTTPServer INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../..
)

target_link_libraries(
        PicoHTTPServer INTERFACE
        pico_neopixel_animations
        pico_multicore
        pico_cyw43_arch_lwip_sys_freertos
        pico_stdlib
        pico_lwip_iperf
        FreeRTOS-Kernel-Heap4
)

if (TARGET Profiler)
    target_link_libraries(
        PicoHTTPServer 
        Profiler
    )
endif()

# Set the appropriate compiler options
set(PICO_BOARD pico_w)
set(WIFI_SSID GC_RGB) #Choose any valid name for network
#set(WIFI_PASSWORD 12345678) #Setting no password leaves the network open

target_compile_definitions(PicoHTTPServer INTERFACE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\")
