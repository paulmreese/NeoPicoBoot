cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Set the appropriate compiler options
set(PICO_BOARD pico_w)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(NeoPicoBoot C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Include FreeRTOS directory
add_subdirectory("PicoWNeoPixelServer/pico-sdk/FreeRTOS/portable/ThirdParty/GCC/RP2040" FREERTOS_KERNEL)
# Add webserver files
add_subdirectory("PicoWNeoPixelServer/PicoHTTPServer")

include(PicoWNeoPixelServer/PicoHttpServer/PicoSDKConfig.cmake)

if(DEFINED SYSPROGS_FRAMEWORKS_FILE)
include(${SYSPROGS_FRAMEWORKS_FILE})
endif()

if (CMAKE_HOST_WIN32)
    set(SIMPLE_FS_BUILDER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/tools/SimpleFSBuilder/SimpleFSBuilder.exe)
else()
    set(SIMPLE_FS_BUILDER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/tools/SimpleFSBuilder/build/SimpleFSBuilder)
endif()

if (NOT EXISTS ${SIMPLE_FS_BUILDER_EXE})
    message(FATAL_ERROR "Missing ${SIMPLE_FS_BUILDER_EXE}. Please build it before building this project.")
endif()

function(add_resource_folder target name path)
	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.fs ${CMAKE_CURRENT_BINARY_DIR}/__rerun_${name}.fs 
		COMMAND ${SIMPLE_FS_BUILDER_EXE}
		ARGS ${path} ${CMAKE_CURRENT_BINARY_DIR}/${name}.fs
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
		COMMENT "Generating ${name}.fs")

	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.o0
		COMMAND ${CMAKE_LINKER}
		ARGS -r -b binary ${name}.fs -o ${CMAKE_CURRENT_BINARY_DIR}/${name}.o0
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}.fs ${CMAKE_CURRENT_BINARY_DIR}/__rerun_${name}.fs 
		COMMENT "Wrapping ${name}.fs")

	add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.o
		COMMAND ${CMAKE_OBJCOPY} 
		ARGS --rename-section .data=.rodata
			 ${CMAKE_CURRENT_BINARY_DIR}/${name}.o0 ${CMAKE_CURRENT_BINARY_DIR}/${name}.o
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}.o0
		COMMENT "Renaming ${name}")

    add_custom_target(${name}-fs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${name}.o)
    add_dependencies(${target} ${name}-fs)
    target_link_libraries(${target} ${CMAKE_CURRENT_BINARY_DIR}/${name}.o)

endfunction()

add_executable(NeoPicoBoot src/picoboot.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/dhcpserver/dhcpserver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/dns/dnsserver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/httpserver.c
        ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/server_settings.c
)

add_resource_folder(NeoPicoBoot www ${CMAKE_CURRENT_SOURCE_DIR}/PicoWNeoPixelServer/PicoHTTPServer/www)

pico_generate_pio_header(NeoPicoBoot  
        ${CMAKE_CURRENT_LIST_DIR}/src/picoboot.pio
)

pico_set_program_name(NeoPicoBoot "NeoPicoBoot")
pico_set_program_version(NeoPicoBoot "0.1")
pico_set_binary_type(NeoPicoBoot default)

pico_enable_stdio_uart(NeoPicoBoot 0)
pico_enable_stdio_usb(NeoPicoBoot 1)

target_compile_definitions(NeoPicoBoot PRIVATE NO_SYS=0)

target_include_directories(NeoPicoBoot PRIVATE src)
target_include_directories(NeoPicoBoot PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(NeoPicoBoot PRIVATE ${CMAKE_CURRENT_LIST_DIR}/PicoWNeoPixelServer/PicoHTTPServer)

target_link_libraries(
        NeoPicoBoot 
        PicoHTTPServer 
        pico_stdlib 
        hardware_pio 
        hardware_dma 
        pico_cyw43_arch_lwip_sys_freertos
        pico_lwip_iperf
)        

pico_add_extra_outputs(NeoPicoBoot)