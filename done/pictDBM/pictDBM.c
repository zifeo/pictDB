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
#include "image_content.h"

#include <string.h>
#include <stdlib.h>
#include <vips/vips.h>

/********************************************************************//**
 * Opens pictDB file and calls do_list command.
 ********************************************************************** */
int do_list_cmd(const char *filename)
{
    if (filename == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    struct pictdb_file myfile;
    int status = do_open(filename, "rb", &myfile);

    if (status == 0) {
        status = do_list(&myfile);
    }

    do_close(&myfile);
    return status;
}

int do_resize_cmd(const char *filename, unsigned int idx, unsigned int res)
{
    if (filename == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    struct pictdb_file myfile;
    int status = do_open(filename, "r+b", &myfile);

    if (status == 0) {
        status = lazy_resize(res, &myfile, idx);
    }

    do_close(&myfile);
    return status;
}

int do_write_cmd(const char *db_name, const char *filename, unsigned int idx, unsigned int res)
{
    if (db_name == NULL || filename == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    struct pictdb_file myfile;
    int status = do_open(db_name, "r+b", &myfile);

    if (status == 0) {
        status = write_image(res, &myfile, filename, idx);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * Prepares and calls do_create command.
********************************************************************** */
int do_create_cmd(const char *filename)
{
    if (filename == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // This will later come from the parsing of command line arguments
    const uint32_t max_files = 10;
    const uint16_t thumb_res = 64;
    const uint16_t small_res = 256;

    puts("Create");
    struct pictdb_file myfile;

    myfile.header.max_files = max_files;

    myfile.header.res_resized[RES_THUMB] = thumb_res;
    myfile.header.res_resized[RES_THUMB + 1] = thumb_res;
    myfile.header.res_resized[2 * RES_SMALL] = small_res;
    myfile.header.res_resized[2 * RES_SMALL + 1] = small_res;

    int status = do_create(filename, &myfile);
    if (status == 0) {
        print_header(&myfile.header);
    }

    return status;
}

/********************************************************************//**
 * Displays some explanations.
 ********************************************************************** */
int help(void)
{
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
int do_delete_cmd(const char *filename, const char *pict_id)
{

    if (filename == NULL || pict_id == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(filename) == 0 || strlen(filename) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    if (strlen(pict_id) == 0 || strlen(pict_id) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    struct pictdb_file myfile;
    int status = do_open(filename, "r+b", &myfile);

    if (status == 0) {
        status = do_delete(pict_id, &myfile);
    }

    do_close(&myfile);
    return status;
}

/********************************************************************//**
 * MAIN
 */
int main(int argc, char *argv[])
{
    if (VIPS_INIT(argv[0])) {
        vips_error_exit("unable to start VIPS");
    }

    int ret = 0;

    if (argc < 2) {
        ret = ERR_NOT_ENOUGH_ARGUMENTS;
    } else {
        /* **********************************************************************
         * TODO WEEK 08: THIS PART SHALL BE REVISED THEN (WEEK 09) EXTENDED.
         * **********************************************************************
         */
        argc--;
        argv++; // skips command call name
        if (!strcmp("list", argv[0])) {
            if (argc < 2) {
                ret = ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                ret = do_list_cmd(argv[1]);
            }
        } else if (!strcmp("resize", argv[0])) {
            if (argc < 4) {
                ret = ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                int index = atoi(argv[2]);
                int res =  atoi(argv[3]);
                ret = do_resize_cmd(argv[1], index, res);
            }
        } else if (!strcmp("write", argv[0])) {
            if (argc < 5) {
                ret = ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                int index = atoi(argv[3]);
                int res =  atoi(argv[4]);
                ret = do_write_cmd(argv[1], argv[2], index, res);
            }
        } else if (!strcmp("create", argv[0])) {
            if (argc < 2) {
                ret = ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                ret = do_create_cmd(argv[1]);
            }
        } else if (!strcmp("delete", argv[0])) {
            if (argc < 3) {
                ret = ERR_NOT_ENOUGH_ARGUMENTS;
            } else {
                ret = do_delete_cmd(argv[1], argv[2]);
            }
        } else if (!strcmp("help", argv[0])) {
            ret = help();
        } else {
            ret = ERR_INVALID_COMMAND;
        }
    }

    if (ret) {
        fprintf(stderr, "ERROR: %s\n", ERROR_MESSAGES[ret]);
        (void) help();
    }

    vips_shutdown();

    return ret;
}
