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

#define CREATE_MAX_FILES "-max_files"
#define CREATE_THUMB_RES "-thumb_res"
#define CREATE_SMALL_RES "-small_res"

#define NAME_RES_THUMB "thumb"
#define NAME_RES_THUMBNAIL "thumbnail"
#define NAME_RES_SMALL "small"
#define NAME_RES_ORIG "orig"
#define NAME_RES_ORIGINAL "original"

#define IMG_EXT ".jpg"

typedef int (*command)(int args, char *argv[]);

struct command_mapping {
    const char name[CMDNAME_MAX];
    command function;
};

/********************************************************************//**
 * Opens pictDB file and calls do_list command.
 ********************************************************************** */
int do_list_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
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
int do_create_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    uint32_t max_files = DEFAULT_MAX_FILES;
    uint16_t thumb_resX = DEFAULT_THUMB_RES;
    uint16_t thumb_resY = DEFAULT_THUMB_RES;
    uint16_t small_resX = DEFAULT_SMALL_RES;
    uint16_t small_resY = DEFAULT_SMALL_RES;

    int i = 2;
    // we skip the function name and first argument
    while (i < argc) {
        if (!strncmp(argv[i], CREATE_MAX_FILES, CMDNAME_MAX)) {
            if (argc <= i + 1) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            }
            max_files = atouint32(argv[i + 1]);
            if (max_files == 0 || max_files > MAX_MAX_FILES) {
                return ERR_MAX_FILES;
            }
            i += 2;
        } else if (!strncmp(argv[i], CREATE_THUMB_RES, CMDNAME_MAX)) {
            if (argc <= i + 2) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            }
            thumb_resX = atouint16(argv[i + 1]);
            thumb_resY = atouint16(argv[i + 2]);
            if (thumb_resX == 0 || thumb_resY == 0 ||
                thumb_resX > MAX_THUMB_RES || thumb_resY > MAX_THUMB_RES) {
                return ERR_RESOLUTIONS;
            }
            i += 3;
        } else if (!strncmp(argv[i], CREATE_SMALL_RES, CMDNAME_MAX)) {
            if (argc <= i + 2) {
                return ERR_NOT_ENOUGH_ARGUMENTS;
            }
            small_resX = atouint16(argv[i + 1]);
            small_resY = atouint16(argv[i + 2]);
            if (small_resX == 0 || small_resY == 0 ||
                small_resX > MAX_SMALL_RES || small_resY > MAX_SMALL_RES) {
                return ERR_RESOLUTIONS;
            }
            i += 3;
        } else {
            return ERR_INVALID_ARGUMENT;
        }
    }

    // thumbnail should be smaller than small ones, or at least a different ratio
    if (thumb_resX >= small_resX && thumb_resY >= small_resY) {
        return ERR_RESOLUTIONS;
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
int help(int argc, char *argv[])
{
    puts("pictDBM [COMMAND] [ARGUMENTS]");
    puts("  help: displays this help.");
    puts("  list <dbfilename>: list pictDB content.");
    puts("  create <dbfilename>: create a new pictDB.");
    puts("      options are:");
    puts("          "CREATE_MAX_FILES" <MAX_FILES>: maximum number of files.");
    printf("                                  default value is %d\n", DEFAULT_MAX_FILES);
    printf("                                  maximum value is %d\n", MAX_MAX_FILES);
    puts("          "CREATE_THUMB_RES" <X_RES> <Y_RES>: resolution for thumbnail images.");
    printf("                                  default value is %dx%d\n", DEFAULT_THUMB_RES, DEFAULT_THUMB_RES);
    printf("                                  maximum value is %dx%d\n", MAX_THUMB_RES, MAX_THUMB_RES);
    puts("          "CREATE_SMALL_RES" <X_RES> <Y_RES>: resolution for small images.");
    printf("                                  default value is %dx%d\n", DEFAULT_SMALL_RES, DEFAULT_SMALL_RES);
    printf("                                  maximum value is %dx%d\n", MAX_SMALL_RES, MAX_SMALL_RES);
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
 * Opens pictDB file and calls do_insert command.
 ********************************************************************** */
int do_insert_cmd(int argc, char *argv[]) {
    if (argc < 4) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    if (strlen(argv[2]) == 0 || strlen(argv[2]) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    if (strlen(argv[3]) == 0 || strlen(argv[3]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    struct pictdb_file myfile;
    int status = do_open(argv[1], "r+b", &myfile);

    if (status == 0) {
        //status = do_insert(argv[2], &myfile);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Opens pictDB file and calls do_read command.
 ********************************************************************** */
int do_read_cmd(int argc, char *argv[]) {
    if (argc < 4) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(argv[1]) == 0 || strlen(argv[1]) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    if (strlen(argv[2]) == 0 || strlen(argv[2]) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    int resolution = -1;

    if ((resolution = resolution_atoi(argv[3])) == -1) {
        return ERR_INVALID_ARGUMENT;
    }

    struct pictdb_file myfile;
    int status = do_open(argv[1], "r+b", &myfile);

    if (status == 0) {
        // TODO update this
        const char **buffer = NULL;
        uint32_t image_size = 0;

        status = do_read(argv[2], (unsigned int) resolution, buffer, &image_size, &myfile);
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

int read_disk_image(void **image_buffer, uint32_t *image_size, const char *filename) {
    if (*image_buffer == NULL || image_size == NULL || filename == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    VipsImage *original = vips_image_new_from_file(filename, NULL);
    if (original == NULL) {
        return ERR_VIPS;
    }


    if (vips_jpegload_buffer(*image_buffer, 0, &original) != 1) {
        return ERR_VIPS;
    }

    return 0;
}

int write_disk_image(char **image_buffer) {
    return 0;
}

int create_name(const char **filename, const char *pic_id, unsigned int res) {
    if (*filename == NULL || pic_id == NULL) {
        return ERR_INVALID_ARGUMENT;
    }
    if (res != RES_ORIG && res != RES_SMALL && res != RES_THUMB) {
        return ERR_RESOLUTIONS;
    }

    strncpy(*filename, pic_id, FILENAME_MAX);
    strncat(*filename, "_", FILENAME_MAX);
    strncat(*filename, res == RES_THUMB ? NAME_RES_THUMB :
                       res == RES_SMALL ? NAME_RES_SMALL :
                       NAME_RES_ORIG,
            FILENAME_MAX);
    strncat(*filename, IMG_EXT, FILENAME_MAX);

    return 0;
}

/********************************************************************//**
 * MAIN
 ********************************************************************** */
int main(int argc, char *argv[])
{
    if (VIPS_INIT(argv[0])) {
        vips_error_exit("unable to start VIPS");
    }


    struct command_mapping commands[] = {
        {"list", do_list_cmd},
        {"create", do_create_cmd},
        {"help", help},
        {"delete", do_delete_cmd},
        {"insert", do_insert_cmd},
        {"read", do_read_cmd}
    };
    const size_t NB_CMD = sizeof(commands) / sizeof(commands[0]);

    int ret = 0;
    if (argc < 2) {
        ret = ERR_NOT_ENOUGH_ARGUMENTS;
    } else {
        argc--;
        argv++; // skips command call name

        command selected_cmd = NULL;
        for (size_t i = 0; i < NB_CMD && selected_cmd == NULL; ++i) {
            if (!strncmp(commands[i].name, argv[0], CMDNAME_MAX)) {
                selected_cmd = commands[i].function;
            }
        }

        if (selected_cmd != NULL) {
            ret = selected_cmd(argc, argv);
        } else {
            // In case we don't find the image, we throw this error code
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
