
# Csound opcode build
#
# ref: https://github.com/csound/opcode_sdk/blob/master/6.x/CMakeLists.txt
# this is a stripped down version of the cmake module ^^

# currently only works on linux!

if(APPLE)
    find_library(CSOUND_LIBRARY CsoundLib
        PATHS
            ~/Library/Frameworks/CsoundLib.framework/CsoundLib
    )

    # Frameworks keep headers in:
    #   CsoundLib.framework/Headers
    # so we find them there
    find_path(CSOUND_INCLUDE_DIR csound.h
        PATHS
            ~/Library/Frameworks/CsoundLib.framework/Headers
    )
else()
    message("Building Csound on linux...")
    find_library(CSOUND_LIBRARY csound
        PATHS /usr/local/lib
    )

    find_path(CSOUND_INCLUDE_DIR csound.h
        PATHS /usr/local/include/csound/
    )
endif()

if(NOT CSOUND_INCLUDE_DIR OR NOT CSOUND_LIBRARY)
    message(FATAL_ERROR "Csound not found!")
endif()

message("csound include dir: ${CSOUND_INCLUDE_DIR}")
message("csound library: ${CSOUND_LIBRARY}")

set(CSND_PLUGIN_LIB blpcsnd)
file(GLOB PLUGIN_SRC_FILES *.c)
add_library(${CSND_PLUGIN_LIB} SHARED ${PLUGIN_SRC_FILES})


# set_target_properties(${CSND_PLUGIN_LIB} PROPERTIES
#     LINK_FLAGS "-Wl,--no-as-needed"
# )
target_compile_options(${CSND_PLUGIN_LIB} PRIVATE -Wextra -Wall)
target_link_options(${CSND_PLUGIN_LIB} PRIVATE -Wno-undef)


target_include_directories(${CSND_PLUGIN_LIB} PRIVATE
    ${CSOUND_INCLUDE_DIR}
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(${CSND_PLUGIN_LIB} ${CSOUND_LIBRARY} ${PROJECT_NAME})
