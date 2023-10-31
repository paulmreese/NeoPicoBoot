cmake_minimum_required(VERSION 3.12)
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(PicoWNeoPixelServer C CXX ASM)
include("PicoHTTPServer/PicoSDKConfig.cmake")
pico_sdk_init()

add_subdirectory("${PICO_SDK_PATH}/FreeRTOS/portable/ThirdParty/GCC/RP2040" FREERTOS_KERNEL)
# Add webserver files
add_subdirectory("PicoHTTPServer")

# Set the appropriate compiler options

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

if(DEFINED SYSPROGS_FRAMEWORKS_FILE)
include(${SYSPROGS_FRAMEWORKS_FILE})
endif()

if (CMAKE_HOST_WIN32)
    set(SIMPLE_FS_BUILDER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/tools/SimpleFSBuilder/SimpleFSBuilder.exe)
else()
    set(SIMPLE_FS_BUILDER_EXE ${CMAKE_CURRENT_SOURCE_DIR}/tools/SimpleFSBuilder/build/SimpleFSBuilder)
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

add_executable(PicoWNeoPixelServer
        example/example.cpp
        PicoHTTPServer/main.cpp
        PicoHTTPServer/dhcpserver/dhcpserver.c
        PicoHTTPServer/dns/dnsserver.c
        PicoHTTPServer/httpserver.c
        PicoHTTPServer/server_settings.c)

add_resource_folder(PicoWNeoPixelServer www ${CMAKE_CURRENT_BINARY_DIR}/../../PicoHTTPServer/www)

pico_enable_stdio_uart(PicoWNeoPixelServer 0)
pico_enable_stdio_usb(PicoWNeoPixelServer 1)

target_compile_definitions(PicoWNeoPixelServer PRIVATE NO_SYS=0)
target_include_directories(PicoWNeoPixelServer PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/PicoHTTPServer)

target_link_libraries(PicoWNeoPixelServer
        PicoHTTPServer)

if (TARGET Profiler)
    target_link_libraries(PicoWNeoPixelServer 
            Profiler)
endif()

pico_add_extra_outputs(PicoWNeoPixelServer)