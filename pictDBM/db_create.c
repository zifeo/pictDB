/**
 * @file db_create.c
 * @brief pictDB library: do_create implementation.
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <string.h>  // for strncpy

/********************************************************************//**
 * Create a new picture database in db_file.
 */
int do_create (const char* filename, struct pictdb_file* db_file)
{
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_NON_NULL(db_file);
    M_REQUIRE_VALID_FILENAME(filename);

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
    db_file->metadata = (struct pict_metadata*) calloc(db_file->header.max_files, sizeof(struct pict_metadata));
    if (db_file->metadata == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;
    db_file->fpdb = fopen(filename, "w+b");

    if (db_file->fpdb == NULL) {
        status = ERR_IO;
    } else {
        if (fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1 ||
            fwrite(db_file->metadata, sizeof(struct pict_metadata), db_file->header.max_files, db_file->fpdb)
            != db_file->header.max_files) {
            status = ERR_IO;
        }
    }

    return status;
}
