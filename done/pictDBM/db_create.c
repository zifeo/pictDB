/**
 * @file db_create.c
 * @brief pictDB library: do_create implementation.
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <string.h> // for strncpy

/********************************************************************//**
 * Creates the database called db_filename. Writes the header and the
 * preallocated empty metadata array to database file.
 */
int do_create (const char* filename, struct pictdb_file* db_file)
{
    if (filename == NULL) {
        return ERR_INVALID_FILENAME;
    }

    if (db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    // Sets the DB header name
    strncpy(db_file->header.db_name, CAT_TXT,  MAX_DB_NAME);
    db_file->header.db_name[MAX_DB_NAME] = '\0';

    // we initialize here all the other fields of the header that were not set explicitly before
    db_file->header.db_version = 0;
    db_file->header.num_files = 0;
    db_file->header.unused_32 = 0;
    db_file->header.unused_64 = 0;

    // now we set all the metadata to 0 so we don't have any surprise and all isValid fields are set to 0
    memset(db_file->metadata, 0, MAX_MAX_FILES * sizeof(struct pict_metadata));

    db_file->fpdb = fopen(filename, "wb");

    if (db_file->fpdb == NULL) {
        return ERR_FILE_NOT_FOUND;
    }

    // Since now the file is open, we need to close it before returning anything
    int status = 0;

    if (1 != fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) ||
        db_file->header.max_files !=
        fwrite(db_file->metadata, sizeof(struct pict_metadata), db_file->header.max_files, db_file->fpdb)) {
        status = ERR_IO;
    }

    if (0 != fclose(db_file->fpdb)) {
        return ERR_IO;
    }

    // The stream is closed, we don't want to print anything in case of an error
    if (status == 0) {
        printf("%d item(s) written\n", 1 + db_file->header.max_files);
    }

    return status;
}
