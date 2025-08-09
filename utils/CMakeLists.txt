# utility clis
# - depends on libsndfile system install
#

# Prefer system packages
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Try to find libsndfile
find_package(SndFile REQUIRED)


# SndFile::sndfile
if (NOT SndFile_FOUND)
    message(FATAL_ERROR "libsndfile not found.")
endif()

message("Found sndfile!")

# make ourselves an executable...
# for now we limit ourselves to one src per exe
function(make_exe name src)

    add_executable(${name}
        ${src}
        cli.c
        wavio.c
    )
    target_link_libraries(${name} PRIVATE ${PROJECT_NAME} m SndFile::sndfile)
    target_compile_options(${name} PRIVATE -Wextra -Wall -Wpedantic -Werror)

endfunction()

# Executables
# -----------
make_exe(hello hello.c)
make_exe(sinesum sinesum.c)
make_exe(blsaw blsaw.c)
make_exe(blsawtab blsawtab.c)
make_exe(smorph smorph.c)
make_exe(smorph2 smorph2.c)
