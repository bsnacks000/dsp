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


# TODO: "render" tests for integration and things like impulse response
# For now we are just using csound for this but it needs to be configured in CI
#
# We would need to develop our own sndfile wrapper (wavio) before we could
# use this ...
#
# find_package(SndFile REQUIRED)

# add_executable(${RENDER_TEST_NAME}
#     munit.c
#     wavio.c
#     render/main.c
# )

# target_link_libraries(${UNIT_TEST_NAME} PRIVATE ${PROJECT_NAME}
#     SndFile::sndfile
# )
# target_compile_options(${UNIT_TEST_NAME} PRIVATE
#     -Wextra -Wall -Wpedantic -Werror
# )

# add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST_NAME})
