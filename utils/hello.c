#include <sndfile.h>
#include <stdio.h>

#include "cli.h"
#include "common.h"

int main(int argc, char** argv) {

    cli_command* c = cli_command_new();

    cli_err err;
    const char* desc = "A useful app";
    const char* usage = "";

    if ((err = cli_init(c, desc, usage, argc, (char**) argv)) != CLI_OK) {
        exit_app_cli_err(c, err);
    }

    if ((err = cli_parse(c)) != CLI_OK) {
        cli_print_help_and_exit(c, err);
    }

    printf("hello world\n");
    exit_app(c);
}
