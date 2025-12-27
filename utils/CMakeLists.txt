# utility clis
# - depends on libsndfile system install
#

find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED IMPORTED_TARGET sndfile)


# make ourselves an executable...
# for now we limit ourselves to one src per exe
function(make_exe name src)

    add_executable(${name}
        ${src}
        cli.c
        wavio.c
    )
    target_link_libraries(${name} PRIVATE ${PROJECT_NAME} m PkgConfig::SNDFILE)
    target_compile_options(${name} PRIVATE -Wextra -Wall -Wpedantic -Werror)

endfunction()

# Executables
# -----------
make_exe(sinesum sinesum.c)
make_exe(blsaw blsaw.c)
make_exe(blsawtab blsawtab.c)
make_exe(smorph smorph.c)
make_exe(smorph2 smorph2.c)
make_exe(chebytab chebytab.c)
#make_exe(wintab wintab.c)
