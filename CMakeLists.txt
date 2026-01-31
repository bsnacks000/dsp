cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME dsp)
project(${PROJECT_NAME} LANGUAGES C VERSION 0.1.1)

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

# objects
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(${PROJECT_NAME}_objects OBJECT)
add_library(${PROJECT_NAME}::objects ALIAS ${PROJECT_NAME}_objects)

target_sources(${PROJECT_NAME}_objects PRIVATE ${SRC_FILES})

target_include_directories(${PROJECT_NAME}_objects
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

target_compile_options(${PROJECT_NAME}_objects
    PRIVATE
        -Wextra -Wall -Wpedantic -Werror
        -Wmissing-declarations -Wmissing-prototypes
)

# lto
#
option(DSP_ENABLE_LTO "Enable LTO if supported." ON)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_err)

if (DSP_ENABLE_LTO)
    message(STATUS "dsp: LTO Supported")
    set_property(TARGET ${PROJECT_NAME}_objects PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "dsp: LTO Not supported : ${lto_err}")
endif()


# library .a/.so
#
option(DSP_BUILD_LIB "Build a static library target by default." ON)
option(DSP_BUILD_SHARED "Build a shared library instead of static." OFF)


if (DSP_BUILD_LIB)
    if (DSP_BUILD_SHARED)
        message(STATUS "dsp: building SHARED library.")
        add_library(${PROJECT_NAME} SHARED)
    else()
        message(STATUS "dsp: building STATIC library.")
        add_library(${PROJECT_NAME} STATIC)
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_objects)

    target_include_directories(${PROJECT_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    )
endif()

# coverage options on the target
option(DSP_COVERAGE "Enable coverage reporting" OFF)

if(DSP_COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Coverage is most useful in Debug mode; forcing Debug build")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(DSP_COVERAGE)
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
if (DSP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# build csound opcodes (currently only linux)
if (DSP_BUILD_CSOUND)
    add_subdirectory(csnd)
endif()

# build the cli utils
if (DSP_BUILD_UTILS)
    add_subdirectory(utils)
endif()
