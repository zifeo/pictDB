/**
 * @file db_delete.c
 * @implementation of do_delete to delete an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 2 April 2016
 */

#include <string.h>
#include "pictDB.h"

int do_delete (const char* pict_id, struct pictdb_file* db_file)
{
    if (pict_id == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (strlen(pict_id) == 0 || strlen(pict_id) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    size_t pict_delete_offset = 0;
    struct pict_metadata* pict_to_delete = NULL;

    // The ending condition also ensures that once we found the correct metadata
    // we do not iterate over the others
    for (size_t i = 0; pict_to_delete == NULL && i < MAX_MAX_FILES; ++i) {
        if (db_file->metadata[i].is_valid == NON_EMPTY &&
            strncmp(db_file->metadata[i].pict_id, pict_id, MAX_PIC_ID) == 0) {
            pict_to_delete = &db_file->metadata[i];
            pict_delete_offset = i * sizeof(struct pict_metadata);
        }
    }

    if (pict_to_delete == NULL) {
        return ERR_FILE_NOT_FOUND;
    }
    pict_to_delete->is_valid = EMPTY;

    // The writing step is done in two parts :
    // 1) Set the new metadata accordingly to what we've done before
    // 2) Update the header

    // By default fwrite start writing at the begining of the stream. Therefore, we
    // need to move the cursor to the position of where we want to delete
    if (fseek(db_file->fpdb, sizeof(struct pictdb_header) + pict_delete_offset, SEEK_SET) != 0
        || fwrite(pict_to_delete, sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    } else {
        // Here we ask ourselves what to do if one fwrite succeeds and one doesn't :
        // this is going to lead to a corruption of the database but TAs say we do not
        // need to handle this corner case : therefore we directly return
        db_file->header.db_version += 1;
        db_file->header.num_files -= 1;
        if (fseek(db_file->fpdb, 0, SEEK_SET) != 0
            || fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {
            return ERR_IO;
        }
    }

    return 0;
}
