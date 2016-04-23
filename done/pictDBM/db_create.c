/**
 * @file db_create.c
 * @brief pictDB library: do_create implementation.
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <stdlib.h>
#include <string.h>  // for strncpy

/********************************************************************//**
 * Creates the database called db_filename. Writes the header and the
 * preallocated empty metadata array to database file.
 */
int do_create (const char* filename, struct pictdb_file* db_file)
{
    if (filename == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(filename) == 0 || strlen(filename) > FILENAME_MAX) {
        return ERR_INVALID_FILENAME;
    }

    // Sets the DB header name
    strncpy(db_file->header.db_name, CAT_TXT, MAX_DB_NAME);
    db_file->header.db_name[MAX_DB_NAME] = '\0';

    // we initialize here all the other fields of the header that were not set explicitly before
    db_file->header.db_version = 0;
    db_file->header.num_files = 0;
    db_file->header.unused_32 = 0;
    db_file->header.unused_64 = 0;

    // now we set all the metadata to 0 so we don't have any surprise and all
    // isValid fields are set to 0
    // TODO calloc max_files or num_files
    db_file->metadata = (struct pict_metadata*) calloc(db_file->header.max_files, sizeof(struct pict_metadata));
    if (db_file->metadata == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;
    db_file->fpdb = fopen(filename, "wb");

    if (db_file->fpdb == NULL) {
        status = ERR_IO;
    } else {
        if (fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1 ||
            fwrite(db_file->metadata, sizeof(struct pict_metadata), db_file->header.max_files, db_file->fpdb)
            != db_file->header.max_files) {
            status = ERR_IO;
        }
    }

    free(db_file->metadata);
    db_file->metadata = NULL;

    if (fclose(db_file->fpdb) != 0 && status == 0) {
        return ERR_IO;
    }

    // The stream is closed, we don't want to print anything in case of an error
    if (status == 0) {
        printf("%d item(s) written\n", 1 + db_file->header.max_files);
    }

    return status;
}
