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
#include "pictDBM_tools.h"

#include <string.h>
#include <vips/vips.h>

#define CMDNAME_MAX 32
#define NB_CMD 4

#define CREATE_MAX_FILES "-max_files"
#define CREATE_THUMB_RES "-thumb_res"
#define CREATE_SMALL_RES "-small_res"

#define NAME_RES_THUMB "thumb"
#define NAME_RES_THUMBNAIL "thumbnail"
#define NAME_RES_SMALL "small"
#define NAME_RES_ORIG "orig"
#define NAME_RES_ORIGINAL "original"

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

    uint32_t max_files = DEFAULT_MAX_FILES;
    uint16_t thumb_resX = DEFAULT_THUMB_RES;
    uint16_t thumb_resY = DEFAULT_THUMB_RES;
    uint16_t small_resX = DEFAULT_SMALL_RES;
    uint16_t small_resY = DEFAULT_SMALL_RES;

    int i = 2;
    // we skip the first argument
    while (i < argc) {
        if (!strcmp(argv[i], CREATE_MAX_FILES)) {
            if (i + 1 >= argc) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                uint32_t tmp_max = atouint32(argv[i + 1]);
                if (tmp_max == 0 || tmp_max > MAX_MAX_FILES) {
                    return ERR_MAX_FILES;
                }
                max_files = tmp_max;
                i += 2;
            }
        } else if (!strcmp(argv[i], CREATE_THUMB_RES)) {
            if (i + 2 >= argc) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                uint16_t tmp_thumb_resX = atouint16(argv[i + 1]);
                uint16_t tmp_thumb_resY = atouint16(argv[i + 2]);
                if (tmp_thumb_resX == 0 || tmp_thumb_resY == 0 ||
                    tmp_thumb_resX > MAX_THUMB_RES || tmp_thumb_resY > MAX_THUMB_RES) {
                    return ERR_RESOLUTIONS;
                }
                thumb_resX = tmp_thumb_resX;
                thumb_resY = tmp_thumb_resY;
                i += 3;
            }
        } else if (!strcmp(argv[i], CREATE_SMALL_RES)) {
            if (i + 2 >= argc) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                uint16_t tmp_small_resX = atouint16(argv[i + 1]);
                uint16_t tmp_small_resY = atouint16(argv[i + 2]);
                if (tmp_small_resX == 0 || tmp_small_resY == 0 ||
                    tmp_small_resX > MAX_SMALL_RES || tmp_small_resY > MAX_SMALL_RES) {
                    return ERR_RESOLUTIONS;
                }
                small_resX = tmp_small_resX;
                small_resY = tmp_small_resY;
                i += 3;
            }
        } else {
            return ERR_INVALID_ARGUMENT;
        }
    }
    struct pictdb_file db_file;

    db_file.header.max_files = max_files;

    db_file.header.res_resized[RES_THUMB] = thumb_resX;
    db_file.header.res_resized[RES_THUMB + 1] = thumb_resY;
    db_file.header.res_resized[2 * RES_SMALL] = small_resX;
    db_file.header.res_resized[2 * RES_SMALL + 1] = small_resY;

    int status = do_create(argv[1], &db_file);
    if (status == 0) {
        print_header(&db_file.header);
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
    puts("    options are:");
    printf("      %s <MAX_FILES>: maximum number of files.\n", CREATE_MAX_FILES);
    printf("\t\t\t\tdefault value is %d\n", DEFAULT_MAX_FILES);
    printf("\t\t\t\tmaximum value is %d\n", MAX_MAX_FILES);
    printf("      %s <X_RES> <Y_RES>: resolution for thumbnail images.\n", CREATE_THUMB_RES);
    printf("\t\t\t\tdefault value is %dx%d\n", DEFAULT_THUMB_RES, DEFAULT_THUMB_RES);
    printf("\t\t\t\tmaximum value is %dx%d\n", MAX_THUMB_RES, MAX_THUMB_RES);
    printf("      %s <X_RES> <Y_RES>: resolution for small images.\n", CREATE_SMALL_RES);
    printf("\t\t\t\tdefault value is %dx%d\n", DEFAULT_SMALL_RES, DEFAULT_SMALL_RES);
    printf("\t\t\t\tmaximum value is %dx%d\n", MAX_SMALL_RES, MAX_SMALL_RES);
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


int resolution_atoi(const char *resolution) {
    if (resolution == NULL) {
        return -1;
    }
    if (!strcmp(NAME_RES_THUMB, resolution) || !strcmp(NAME_RES_THUMBNAIL, resolution)) {
        return RES_THUMB;
    }
    if (!strcmp(NAME_RES_SMALL, resolution)) {
        return RES_SMALL;
    }
    if (!strcmp(NAME_RES_ORIG, resolution) || !strcmp(NAME_RES_ORIGINAL, resolution)) {
        return RES_ORIG;
    }
    return -1;
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

        // In case we don't find the image, we throw this error code
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
