cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME dsp)
project(dsp LANGUAGES C VERSION 0.2.0)

# extend semver prerelease tag .. Ex. -alpha.0, -rc.1
set(DSP_VERSION "${PROJECT_VERSION}")

# generate and add to build tree
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/dsp/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/dsp/version.h"
    @ONLY
)

message(STATUS "Generated version.h: ${DSP_VERSION}")

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

# objects lib
# use as dsp::objects
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(dsp_objects OBJECT)
add_library(dsp::objects ALIAS dsp_objects)

target_sources(dsp_objects PRIVATE ${SRC_FILES})

target_include_directories(dsp_objects
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_compile_options(dsp_objects
    PRIVATE
        -Wextra -Wall -Wpedantic -Werror
        -Wmissing-declarations -Wmissing-prototypes
)


# build the tests
if (DSP_BUILD_TESTS)
    enable_testing()
    if(DSP_COVERAGE)
        if(CMAKE_C_COMPILER_ID MATCHES "Clang")
            target_compile_options(dsp_objects PUBLIC
                -fprofile-instr-generate -fcoverage-mapping
            )
            target_link_options(dsp_objects PUBLIC
                -fprofile-instr-generate -fcoverage-mapping
            )
        elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
            target_compile_options(dsp_objects PUBLIC
                --coverage -O0 -g
            )
            target_link_options(dsp_objects PUBLIC
                --coverage
            )
        endif()
    endif()
    add_subdirectory(tests)
endif()

# build csound opcodes
if (DSP_BUILD_CSOUND)
    add_subdirectory(csnd)
endif()
