cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)


project(mong_proto C CXX ASM)
set(CMAKE_C_STANDARD 11)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_executable(mong_proto
    main.c
)

target_include_directories(mong_proto 
    PUBLIC
    Disp
    Button
    Sound
    Mode
)

target_link_libraries(mong_proto
    pico_stdlib 
    pico_rand 
    hardware_i2c 
    hardware_pwm 
    pico_multicore
    Disp_Ssd1306_ssd1306
)

pico_enable_stdio_usb(mong_proto 1) 
pico_enable_stdio_uart(mong_proto 0) 

pico_add_extra_outputs(mong_proto)

# Add blink example

message("> Debug")
message("Mong_the_spire")
message(${Disp_Ssd1306_ssd1306})