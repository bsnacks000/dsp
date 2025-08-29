cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME dsp)
project(${PROJECT_NAME} LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set default build type if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Export compile commands for tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable position independent code for libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (APPLE)
    message("Building on Apple.")
    add_compile_definitions(-DMACOS=1)
endif()


option(COVERAGE "Enable coverage reporting" OFF)

if(COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Coverage is most useful in Debug mode; forcing Debug build")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()


# always build the main static library
file(GLOB_RECURSE SRC_FILES src/*.c)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_compile_options(${PROJECT_NAME} PRIVATE -Wextra -Wall -Wpedantic -Werror)

if(COVERAGE)
    message(STATUS "Building with coverage instrumentation")
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(${PROJECT_NAME} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
        target_link_options(${PROJECT_NAME} PRIVATE -fprofile-instr-generate -fcoverage-mapping)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE --coverage -O0 -g)
        target_link_options(${PROJECT_NAME} PRIVATE --coverage)
    else()
        message(FATAL_ERROR "Coverage not supported with this compiler")
    endif()
endif()

# build the tests
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# build csound opcodes (currently only linux)
if (BUILD_CSOUND)
    add_subdirectory(csnd)
endif()

# build the cli utils
if (BUILD_UTILS)
    add_subdirectory(utils)
endif()
