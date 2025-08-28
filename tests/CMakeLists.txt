set(UNIT_TEST_NAME dsp_unit_tests)
set(RENDER_TEST_NAME dsp_rendering_tests)

file(GLOB_RECURSE SRC_FILES *.c)

add_executable(${UNIT_TEST_NAME}
    ${SRC_FILES}
)
target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${PROJECT_NAME} m)
target_compile_options(${UNIT_TEST_NAME} PRIVATE
    -Wextra -Wall -Wpedantic -Werror
)

if(COVERAGE)
    target_compile_options(${UNIT_TEST_NAME} PRIVATE -O0 -g --coverage -fkeep-inline-functions -fkeep-static-functions)
    target_link_options(${UNIT_TEST_NAME} PRIVATE --coverage)
endif()
