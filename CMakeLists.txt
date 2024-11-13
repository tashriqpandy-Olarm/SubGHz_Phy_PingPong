cmake_minimum_required(VERSION 3.23)
project(YourMainProject VERSION 1.0 LANGUAGES C CXX)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Setup compiler settings for C++
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch the olarm_protocol_y repository
include(FetchContent)

FetchContent_Declare(
  olarm_protocol_y
  GIT_REPOSITORY git@github.com:OlarmTech/olarm_protocol_y.git
  GIT_TAG        Submodule-tweaking
)
FetchContent_MakeAvailable(olarm_protocol_y)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME SubGHz_Phy_PingPong)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
# target_link_libraries(${CMAKE_PROJECT_NAME}
#     stm32cubemx

#     # Add user defined libraries
# )
# Link only the ProtocolYMessages target to your project
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    LoraInterface
    ProtocolYReturnCodes
    ProtocolYInterface
    ProtocolYManager
    ProtocolYMessages 
    ProtocolYManager
    AESEncryption
    stm32cubemx
)