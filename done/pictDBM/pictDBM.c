/**
 * @file pictDBM.c
 * @brief pictDB Manager: command line interpretor for pictDB core commands.
 *
 * Picture Database Management Tool
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <string.h>
#include <vips/vips.h>

#define CMDNAME_MAX 32
#define NB_CMD 4

typedef int (*command)(int args, char *argv[]);

typedef struct {
    command function;
    const char name[CMDNAME_MAX];
} command_mapping;


/********************************************************************//**
 * Opens pictDB file and calls do_list command.
 ********************************************************************** */
int do_list_cmd(int argc, char *argv[]) {
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    struct pictdb_file myfile;
    int status = do_open(argv[1], "rb", &myfile);

    if (status == 0) {
        status = do_list(&myfile);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Prepares and calls do_create command.
********************************************************************** */
int do_create_cmd(int argc, char *argv[]) {
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // This will later come from the parsing of command line arguments
    const uint32_t max_files = 10;
    const uint16_t thumb_res = 64;
    const uint16_t small_res = 256;

    struct pictdb_file myfile;

    myfile.header.max_files = max_files;

    myfile.header.res_resized[RES_THUMB] = thumb_res;
    myfile.header.res_resized[RES_THUMB + 1] = thumb_res;
    myfile.header.res_resized[2 * RES_SMALL] = small_res;
    myfile.header.res_resized[2 * RES_SMALL + 1] = small_res;

    int status = do_create(argv[1], &myfile);
    if (status == 0) {
        print_header(&myfile.header);
    }

    return status;
}

/********************************************************************//**
 * Displays some explanations.
 ********************************************************************** */
int help(int argc, char *argv[]) {
    puts("pictDBM [COMMAND] [ARGUMENTS]");
    puts("  help: displays this help.");
    puts("  list <dbfilename>: list pictDB content.");
    puts("  create <dbfilename>: create a new pictDB.");
    puts("  delete <dbfilename> <pictID>: delete picture pictID from pictDB.");
    return 0;
}

/********************************************************************//**
 * Deletes a picture from the database.
 */
int do_delete_cmd(int argc, char *argv[]) {
    if (argc < 3) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL || argv[2] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    if (strlen(argv[2]) == 0 || strlen(argv[2]) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    struct pictdb_file myfile;
    int status = do_open(argv[1], "r+b", &myfile);

    if (status == 0) {
        status = do_delete(argv[2], &myfile);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * MAIN
 */
int main(int argc, char *argv[]) {
    if (VIPS_INIT(argv[0])) {
        vips_error_exit("unable to start VIPS");
    }

    command_mapping commands[] = {
            {do_list_cmd,   "list"},
            {do_create_cmd, "create"},
            {help,          "help"},
            {do_delete_cmd, "delete"}
    };

    int ret = 0;

    if (argc < 2) {
        ret = ERR_NOT_ENOUGH_ARGUMENTS;
    } else {
        argc--;
        argv++; // skips command call name

        int i = 0;

        for (i = 0; i < NB_CMD; ++i) {
            if (!strcmp(commands[i].name, argv[0])) {
                ret = commands[i].function(argc, argv);
                break;
            }
        }

        if (i == NB_CMD) {
            ret = ERR_INVALID_COMMAND;
        }
    }
    if (ret) {
        fprintf(stderr, "ERROR: %s\n", ERROR_MESSAGES[ret]);
        (void) help(argc, argv);
    }

    vips_shutdown();

    return ret;
}
