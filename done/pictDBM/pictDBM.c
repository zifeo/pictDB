/**
 * @file pictDBM.c
 * @brief pictDB Manager: command line interpretor for pictDB core commands.
 *
 * Picture Database Management Tool
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 2 Nov 2015
 */

#include "pictDB.h"
#include "pictDBM_tools.h"

#include <string.h>
#include <vips/vips.h>
#include <assert.h>

#define CMDNAME_MAX 32

#define CREATE_MAX_FILES "-max_files"
#define CREATE_THUMB_RES "-thumb_res"
#define CREATE_SMALL_RES "-small_res"

#define IMG_EXT ".jpg"

#define TOKEN_SEPARATOR " "
#define CMD_NAME_POSITION 1
#define INTERPRETOR_EXIT "quit"

#define INPUT_LENGTH 100

typedef int (*command)(int args, char *argv[]);

struct command_mapping {
    const char name[CMDNAME_MAX]; /**< command name */
    command function; /**< command pointer function */
};

/********************************************************************//**
 * Opens pictDB file and calls do_list command.
 ********************************************************************** */
static int do_list_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_VALID_FILENAME(argv[1]);

    const char *db_filename = argv[1];

    struct pictdb_file myfile;
    int status = do_open(db_filename, "rb", &myfile);

    if (status == 0) {
        char *listing = do_list(&myfile, STDOUT);
        if (listing != NULL) {
            // should never happen
            free(listing);
            listing = NULL;
            status = ERR_DEBUG;
        }
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Prepares and calls do_create command.
 ********************************************************************** */
static int do_create_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_VALID_FILENAME(argv[1]);

    const char *db_filename = argv[1];

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

    int status = do_create(db_filename, &db_file);
    if (status == 0) {
        printf("%d item(s) written\n", 1 + db_file.header.max_files);
        print_header(&db_file.header);
    }

    do_close(&db_file);
    return status;
}

/********************************************************************//**
 * Create filename from resolution.
 ********************************************************************** */
static int create_name(char *filename, const char *pic_id, unsigned int res)
{
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_NON_NULL(pic_id);

    filename[0] = '\0';
    strncat(filename, pic_id, FILENAME_MAX);
    strncat(filename, "_", FILENAME_MAX);

    switch (res) {
    case RES_THUMB:
        strncat(filename, NAME_RES_THUMB, FILENAME_MAX);
        break;
    case RES_SMALL:
        strncat(filename, NAME_RES_SMALL, FILENAME_MAX);
        break;
    case RES_ORIG:
        strncat(filename, NAME_RES_ORIG, FILENAME_MAX);
        break;
    default:
        filename[0] = '\0';
        return ERR_RESOLUTIONS;
    }

    strncat(filename, IMG_EXT, FILENAME_MAX);
    return 0;
}

/********************************************************************//**
 * Displays some explanations.
 ********************************************************************** */
static int help(int argc, char *argv[])
{
    (void) argc, (void) argv;
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
    puts("  read   <dbfilename> <pictID> ["NAME_RES_ORIGINAL"|"NAME_RES_ORIG"|"NAME_RES_THUMBNAIL"|"NAME_RES_THUMB"|"
         NAME_RES_SMALL"]:");
    puts("      read an image from the pictDB and save it to a file.");
    puts("      default resolution is \""NAME_RES_ORIGINAL"\".");
    puts("  insert <dbfilename> <pictID> <filename>: insert a new image in the pictDB.");
    puts("  delete <dbfilename> <pictID>: delete picture pictID from pictDB.");
    puts("  gc <dbfilename> <tmpfilename>: collect garbage over the given file,"
         " in case of error, the original file is not modified but only the temporary one.");
    puts("  interpretor <dbfilename>: run an interpretor to perform above operations on a pictDB file.");
    return 0;
}

/********************************************************************//**
 * Deletes a picture from the database.
 ********************************************************************** */
static int do_delete_cmd(int argc, char *argv[])
{
    if (argc < 3) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_NON_NULL(argv[2]);
    M_REQUIRE_VALID_FILENAME(argv[1]);
    M_REQUIRE_VALID_PIC_ID(argv[2]);

    const char *db_filename = argv[1];
    const char *pict_id = argv[2];

    struct pictdb_file myfile;
    int status = do_open(db_filename, "r+b", &myfile);

    if (status == 0) {
        status = do_delete(pict_id, &myfile);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Reads image from disk into buffer.
 ********************************************************************** */
static int read_disk_image(char *image_buffer[], uint32_t *image_size, const char *filename)
{
    M_REQUIRE_NON_NULL(image_buffer);
    M_REQUIRE_NON_NULL(image_size);
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_VALID_FILENAME(filename);

    if (*image_buffer != NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    FILE *image_file = fopen(filename, "rb");
    if (image_file == NULL) {
        return ERR_IO;
    }

    int status = 0;
    if (fseek(image_file, 0, SEEK_END) != 0) {
        status = ERR_IO;
    } else {
        long end_pos = ftell(image_file);
        if (end_pos == -1) {
            status = ERR_IO;
        } else {
            assert(0 <= end_pos);
            size_t size = (size_t) end_pos;

            *image_buffer = malloc(size);
            if (*image_buffer == NULL) {
                status = ERR_OUT_OF_MEMORY;
            } else {

                if (fseek(image_file, 0, SEEK_SET) != 0 ||
                    fread((void *) *image_buffer, size, 1, image_file) != 1) {
                    status = ERR_IO;
                } else {
                    *image_size = (uint32_t) end_pos;
                }
            }
        }
    }

    if (fclose(image_file) != 0 && status == 0) {
        status = ERR_IO;
    }

    if (status != 0 && *image_buffer != NULL) {
        free(*image_buffer);
        *image_buffer = NULL;
    }

    return status;
}

/********************************************************************//**
 * Writes image from buffer to disk.
 ********************************************************************** */
static int write_disk_image(char image_buffer[], uint32_t image_size, const char *filename)
{
    M_REQUIRE_NON_NULL(image_buffer);
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_VALID_FILENAME(filename);

    FILE *image_file = fopen(filename, "wb");
    if (image_file == NULL) {
        return ERR_IO;
    }

    int status = 0;
    if (fwrite(image_buffer, image_size, 1, image_file) != 1) {
        status = ERR_IO;
    }

    if (fclose(image_file) != 0 && status == 0) {
        status = ERR_IO;
    }
    return status;
}

/********************************************************************//**
 * Opens pictDB file and calls do_insert command.
 ********************************************************************** */
static int do_insert_cmd(int argc, char *argv[])
{
    if (argc < 4) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_NON_NULL(argv[2]);
    M_REQUIRE_NON_NULL(argv[3]);
    M_REQUIRE_VALID_FILENAME(argv[1]);
    M_REQUIRE_VALID_PIC_ID(argv[2]);
    M_REQUIRE_VALID_FILENAME(argv[3]);

    const char *db_filename = argv[1];
    const char *pic_id = argv[2];
    const char *filename = argv[3];

    struct pictdb_file myfile;
    int status = do_open(db_filename, "r+b", &myfile);

    if (status == 0) {
        if (myfile.header.num_files >= myfile.header.max_files) {
            status = ERR_FULL_DATABASE;
        } else {
            char *image_buffer = NULL;
            uint32_t image_size = 0;

            status = read_disk_image(&image_buffer, &image_size, filename);
            if (status == 0) {
                assert(image_buffer != NULL);
                status = do_insert(image_buffer, image_size, pic_id, &myfile);
            }

            if (image_buffer != NULL) {
                free(image_buffer);
                image_buffer = NULL;
            }
        }
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Opens pictDB file and calls do_read command.
 ********************************************************************** */
static int do_read_cmd(int argc, char *argv[])
{
    if (argc < 3) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_NON_NULL(argv[2]);
    M_REQUIRE_VALID_FILENAME(argv[1]);
    M_REQUIRE_VALID_PIC_ID(argv[2]);

    const char *db_filename = argv[1];
    const char *pic_id = argv[2];

    int resolution_parsed = argc == 3 ? RES_ORIG : resolution_atoi(argv[3]);
    if (resolution_parsed < 0) {
        return ERR_INVALID_ARGUMENT;
    }
    const unsigned int resolution = (unsigned int) resolution_parsed;

    struct pictdb_file db_file;
    int status = do_open(db_filename, "r+b", &db_file);

    if (status == 0) {
        char *image_buffer = NULL;
        uint32_t image_size = 0;
        status = do_read(pic_id, resolution, &image_buffer, &image_size, &db_file);

        if (status == 0) {
            assert(image_buffer != NULL);
            char filename[FILENAME_MAX];
            status = create_name(filename, pic_id, resolution);
            assert(strlen(filename) > 0);

            if (status == 0) {
                status = write_disk_image(image_buffer, image_size, filename);
            }
        }

        if (image_buffer != NULL) {
            free(image_buffer);
            image_buffer = NULL;
        }

        assert(image_buffer == NULL);
    }
    do_close(&db_file);
    return status;
}

/********************************************************************//**
 * Opens pictDB file and calls do_read command.
 ********************************************************************** */
int do_gbcollect_cmd(int argc, char *argv[])
{
    if (argc < 3) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);
    M_REQUIRE_NON_NULL(argv[2]);
    M_REQUIRE_VALID_FILENAME(argv[1]);
    M_REQUIRE_VALID_FILENAME(argv[2]);

    const char *db_filename = argv[1];
    const char *tmp_db_filename = argv[2];

    struct pictdb_file db_file;
    int status = do_open(db_filename, "rb", &db_file);

    if (status == 0) {
        status = do_gbcollect(&db_file, db_filename, tmp_db_filename);
    }

    do_close(&db_file);
    return status;
}

/********************************************************************//**
 * Displays some explanations.
 ********************************************************************** */
static int interpretor_help(int argc, char *argv[])
{
    (void) argc, (void) argv;
    puts("[COMMAND] [ARGUMENTS]");
    puts("  help: displays this help.");
    puts("  list: list pictDB content.");
    puts("  read <pictID> ["NAME_RES_ORIGINAL"|"NAME_RES_ORIG"|"NAME_RES_THUMBNAIL"|"NAME_RES_THUMB"|"
         NAME_RES_SMALL"]:");
    puts("      read an image from the pictDB and save it to a file.");
    puts("      default resolution is \""NAME_RES_ORIGINAL"\".");
    puts("  insert <pictID> <filename>: insert a new image in the pictDB.");
    puts("  delete <pictID>: delete picture pictID from pictDB.");
    puts("  gc <tmpfilename>: collect garbage over the given file,"
         " in case of error, the original file is not modified but only the temporary one.");
    return 0;
}

/********************************************************************//**
 * Return the command associated to the given name, NULL if not found
 ********************************************************************** */
command get_cmd(struct command_mapping *commands, size_t size, const char *cmd_name)
{
    if (commands != NULL && cmd_name != NULL) {
        for (size_t cmd_no = 0; cmd_no < size; ++cmd_no) {
            if (!strncmp(commands[cmd_no].name, cmd_name, CMDNAME_MAX)) {
                return commands[cmd_no].function;
            }
        }
    }
    return NULL;
}

/********************************************************************//**
 * Tokenize the input string, by separating on space, and add the
 * name of the db_file at the second position
 ********************************************************************** */
int tokenize_input(char *buffer[], char *input, const char *filename)
{
    M_REQUIRE_NON_NULL(buffer);
    M_REQUIRE_NON_NULL(input);
    M_REQUIRE_VALID_FILENAME(filename);

    int argv = 0;
    char *token = NULL;
    // remove end of line
    input[strcspn(input, "\n")] = '\0';

    // check for space first (strtok does not handle this corner case correctly
    if (strstr(input, TOKEN_SEPARATOR) == NULL) {
        buffer[argv] = input;
        argv++;
    } else {
        token = strtok(input, TOKEN_SEPARATOR);
        while (token != NULL) {
            buffer[argv++] = token;
            if (argv == CMD_NAME_POSITION) {
                argv++;
            }
            token = strtok(NULL, TOKEN_SEPARATOR);
        }
    }

    strncpy(buffer[CMD_NAME_POSITION], filename, strlen(filename) + 1);
    return argv;
}

/********************************************************************//**
 * Deal the given command with the given arguments
 ********************************************************************** */
int deal_with_cmd(command cmd, int argc, char *argv[])
{
    if (cmd == NULL) {
        return ERR_INVALID_COMMAND;
    }
    return cmd(argc + 1, argv);
}

/********************************************************************//**
 * Run the interpretor on the given file
 ********************************************************************** */
int do_interpretor_cmd(int argc, char *argv[])
{
    if (argc < 2) {
        return ERR_NOT_ENOUGH_ARGUMENTS;
    }

    M_REQUIRE_NON_NULL(argv[1]);

    const char *db_filename = argv[1];

    // this is a fast but dirty way to check whether or not the given db is on disk
    struct pictdb_file db_file;
    int status = do_open(db_filename, "r+b", &db_file);

    if (status != 0) {
        return status;
    }
    do_close(&db_file);

    // we need new commands as we can't do all the original ones (no creation, no interpretor inside it etc ...)
    struct command_mapping commands[] = {
        {"list",   do_list_cmd},
        {"help",   interpretor_help},
        {"delete", do_delete_cmd},
        {"insert", do_insert_cmd},
        {"read",   do_read_cmd},
        {"gc",     do_gbcollect_cmd}
    };

    const size_t nb_cmd = sizeof(commands) / sizeof(commands[0]);

    char *cmd_argv[MAX_CMD_ARGS];
    int exit = 0;
    char *str = (char *) malloc(INPUT_LENGTH + 1);

    puts("PictDBM interpretor\n"
         "Type \"help\" for more information.");

    do {
        printf(">>> ");
        size_t input_length = INPUT_LENGTH;
        if (getline(&str, &input_length, stdin) == -1) {
            fprintf(stderr, "ERROR: can't read your input\n");
        } else {
            int ret = 0;
            int cmd_argc = tokenize_input(cmd_argv, str, db_filename);

            if (cmd_argc > 0) {
                char *cmd_name = cmd_argv[0];
                exit = strcmp(cmd_name, INTERPRETOR_EXIT);
                command selected_cmd = get_cmd(commands, nb_cmd, cmd_name);
                ret = deal_with_cmd(selected_cmd, cmd_argc + 1, cmd_argv);
            }
            if (ret) {
                fprintf(stderr, "ERROR: %s\n", ERROR_MESSAGES[ret]);
                interpretor_help(argc, argv);
            }
        }
    } while (exit);

    free(str);
    str = NULL;
    return status;
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
        {"list",        do_list_cmd},
        {"create",      do_create_cmd},
        {"help",        help},
        {"delete",      do_delete_cmd},
        {"insert",      do_insert_cmd},
        {"read",        do_read_cmd},
        {"gc",          do_gbcollect_cmd},
        {"interpretor", do_interpretor_cmd},
    };
    const size_t nb_cmd = sizeof(commands) / sizeof(commands[0]);

    int ret = 0;
    if (argc < 2) {
        ret = ERR_NOT_ENOUGH_ARGUMENTS;
    } else {
        argc--;
        argv++; // skips command call name

        command selected_cmd = get_cmd(commands, nb_cmd, argv[0]);
        ret = deal_with_cmd(selected_cmd, argc, argv);
    }

    if (ret) {
        fprintf(stderr, "ERROR: %s\n", ERROR_MESSAGES[ret]);
        (void) help(argc, argv);
    }

    vips_shutdown();
    return ret;
}
