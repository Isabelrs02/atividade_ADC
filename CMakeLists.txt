
# Minimum required CMake version
cmake_minimum_required(VERSION 3.13)

# Default standards for C and C++
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuration for Raspberry Pi Pico SDK and VS Code Extension
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# Define the board (Raspberry Pi Pico W)
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Import the Pico SDK
include(pico_sdk_import.cmake)

# Project name
project(atividade_ADC C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add the executable and source files
add_executable(atividade_ADC 
    atividade_ADC.c 
    include/ssd1306.c  # If there is a corresponding .c file for the library
)

# Set program name and version
pico_set_program_name(atividade_ADC "atividade_ADC")
pico_set_program_version(atividade_ADC "0.1")

# Enable output via UART and USB
pico_enable_stdio_uart(atividade_ADC 1)
pico_enable_stdio_usb(atividade_ADC 1)

# Link required libraries
target_link_libraries(atividade_ADC
    pico_stdlib
    hardware_i2c      # Communication with SSD1306 display
    hardware_pwm      # LED control via PWM
    hardware_adc      # Reading the analog joystick (VRx and VRy)
    hardware_irq      # Interrupts for buttons
    hardware_timer    # Timers
    hardware_clocks   # Clock configuration
)

# Add include directories for the compiler
target_include_directories(atividade_ADC PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    include/  # Directory containing "ssd1306.h" and "font.h"
)

# Generate final binary files (.uf2, .hex, etc.)
pico_add_extra_outputs(atividade_ADC)
