set(UNIT_TEST_NAME dsp_unit_tests)

file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)
target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${PROJECT_NAME}::objects m )
target_include_directories(${UNIT_TEST_NAME} PUBLIC)
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
