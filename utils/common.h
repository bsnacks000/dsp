#ifndef CLI_UTILS_COMMON_H
#define CLI_UTILS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"

/**
 * @brief destroy cli, log err, exit(1)
 *  - called in main if any app returns with an err.
 *  - message handler should be specific to the app
 */
static inline void exit_app_err(cli_command* cli, const char* message) {
    cli_command_destroy(cli);
    fprintf(stderr, "error: %s", message);
    exit(EXIT_FAILURE);
}

/**
 * @brief destroy cli; exit(0)
 *  - always called in main
 */
static inline void exit_app(cli_command* cli) {
    cli_command_destroy(cli);
    exit(EXIT_SUCCESS);
}

/**
 * @brief destory cli; log cli err; exit(1)
    - This is used for handling cli parse errors
 */
static inline void exit_app_cli_err(cli_command* cli, cli_err err) {
    cli_command_destroy(cli);
    cli_print_err(err);
    exit(EXIT_FAILURE);
}

/**
 * @brief
 */
static inline bool validate_path(const char* path, const char* ext) {
    // at least 4 chars
    uint32_t len = strlen(path);
    // check extension
    if (len >= 4 && strcmp(path + len, ext))
        return true;
    return false;
}

#ifdef __cplusplus
}
#endif

#endif
