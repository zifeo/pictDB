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

#include <stdlib.h>
#include <string.h>

/********************************************************************//**
 * Opens pictDB file and calls do_list command.
 ********************************************************************** */
int do_list_cmd (const char* filename)
{
    struct pictdb_file myfile;

    int open_status = do_open(filename, "rb", &myfile);
    if (0 != open_status) {
        return open_status; // TODO
    }

    do_list(&myfile);
    do_close(&myfile); // TODO
    return 0;
}

/********************************************************************//**
 * Prepares and calls do_create command.
********************************************************************** */
int do_create_cmd (const char* filename)
{
    // This will later come from the parsing of command line arguments
    const uint32_t max_files =  10;
    const uint16_t thumb_res =  64;
    const uint16_t small_res = 256;

    puts("Create");
    struct pictdb_file myfile;

    myfile.header.max_files = max_files;

    myfile.header.res_resized[RES_THUMB] = thumb_res;
    myfile.header.res_resized[RES_THUMB + 1] = thumb_res;
    myfile.header.res_resized[2 * RES_SMALL] = small_res;
    myfile.header.res_resized[2 * RES_SMALL + 1] = small_res;

    int create_status = do_create(filename, &myfile);
    if (0 == create_status) {
        print_header(&myfile.header);
    }

    return create_status;
}

/********************************************************************//**
 * Displays some explanations.
 ********************************************************************** */
int help (void)
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
int do_delete_cmd (const char* filename, const char* pictID)
{

    if (filename == NULL) {
        return ERR_INVALID_FILENAME;
    }

    if (pictID == NULL || strlen(pictID) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    return do_delete(pictID, );
}

/********************************************************************//**
 * MAIN
 */
int main (int argc, char* argv[])
{
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
        (void)help();
    }

    return ret;
}
