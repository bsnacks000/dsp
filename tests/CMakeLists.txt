set(UNIT_TEST_NAME dsp_tests)

# static lib
message(STATUS "dsp: building static lib for tests.")
add_library(dsp STATIC)
target_link_libraries(dsp PUBLIC dsp::objects)
target_include_directories(dsp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
)

# coverage options on the target
option(DSP_COVERAGE "Enable coverage reporting" OFF)

if(DSP_COVERAGE AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(WARNING "Coverage is most useful in Debug mode; forcing Debug build")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(DSP_COVERAGE)
  message(STATUS "Building with coverage instrumentation")
  if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(dsp PUBLIC -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(dsp PUBLIC -fprofile-instr-generate -fcoverage-mapping)
  elseif(CMAKE_C_COMPILER_ID MATCHES "GNU")
    target_compile_options(dsp PUBLIC --coverage -O0 -g)
    target_link_options(dsp PUBLIC --coverage)
  else()
    message(FATAL_ERROR "Coverage not supported with this compiler")
  endif()
endif()

# build the test suite
# link against static lib
file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)
target_link_libraries(${UNIT_TEST_NAME} PRIVATE dsp m )
target_compile_options(${UNIT_TEST_NAME} PRIVATE
    -Wextra -Wall -Wpedantic -Werror
)

# coverage options are compiler specific
if(DSP_COVERAGE)
  if(CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    target_compile_options(${UNIT_TEST_NAME} PRIVATE -O0 -g --coverage)
    target_link_options(${UNIT_TEST_NAME} PRIVATE --coverage)
  else()
    message(FATAL_ERROR "Coverage not supported with this compiler: ${CMAKE_C_COMPILER_ID}")
  endif()
endif()
