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

    if (pict_id == NULL || strlen(pict_id) > MAX_PIC_ID) {
        return ERR_INVALID_PICID;
    }

    if (db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    struct pict_metadata* pict_to_delete = NULL;
    size_t pict_position = 0;

    for (size_t i = 0; pict_to_delete == NULL && i < MAX_MAX_FILES; ++i) {
        if (db_file->metadata[i].is_valid == NON_EMPTY &&
            db_file->metadata[i].pict_id == pict_id) {
            pict_to_delete = &db_file->metadata[i];
            pict_position = i;
        }
    }

    if (pict_to_delete == NULL) {
        return ERR_INVALID_PICID;
    }
    pict_to_delete->is_valid = EMPTY;

    int status = 0;

    if (0 != fseek(db_file->fpdb, sizeof(struct pictdb_header) + pict_position, SEEK_SET)) {
        status = ERR_IO;
    } else if (1 != fwrite(pict_to_delete, sizeof(struct pict_metadata), 1, db_file->fpdb)) {
        db_file->header.db_version += 1;
        db_file->header.num_files -= 1;

        if (1 != fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb)) {
            status = ERR_IO;
        }
    }

    return status;

}
