/**
 * @file dedup.c
 * @implementation of do_name_and_content_dedup to avoid storing the same image multiple times
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 23 April 2016
 */

#include "dedup.h"
#include <string.h>

/********************************************************************//**
 * Check for SHA-1 and name duplication and optimize those cases.
 */
int do_name_and_content_dedup(struct pictdb_file *db_file, const uint32_t index)
{
    M_REQUIRE_NON_NULL(db_file);

    // TODO : verify >
    if (index > db_file->header.max_files ||
        db_file->metadata[index].is_valid == EMPTY) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    uint32_t similar_sha = db_file->header.max_files;

    for (uint32_t i = 0; i < db_file->header.max_files; ++i) {
        if (i != index && db_file->metadata[i].is_valid == NON_EMPTY) {
            if (!strncmp(db_file->metadata[i].pict_id, db_file->metadata[index].pict_id, MAX_PIC_ID)) {
                return ERR_DUPLICATE_ID;
            } else if (!memcmp(db_file->metadata[i].SHA, db_file->metadata[index].SHA, SHA256_DIGEST_LENGTH)) {
                similar_sha = i;
            }
        }
    }

    if (similar_sha != db_file->header.max_files) {
        db_file->metadata[index].offset[RES_THUMB] = db_file->metadata[similar_sha].offset[RES_THUMB];
        db_file->metadata[index].offset[RES_SMALL] = db_file->metadata[similar_sha].offset[RES_SMALL];
        db_file->metadata[index].offset[RES_ORIG] = db_file->metadata[similar_sha].offset[RES_ORIG];
        db_file->metadata[index].size[RES_THUMB] = db_file->metadata[similar_sha].size[RES_THUMB];
        db_file->metadata[index].size[RES_SMALL] = db_file->metadata[similar_sha].size[RES_SMALL];

        if (fseek(db_file->fpdb, sizeof(struct pictdb_header) + index * sizeof(struct pict_metadata), SEEK_SET) != 0 ||
            fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {

            return ERR_IO;
        }
        return 0;
    }

    db_file->metadata[index].offset[RES_ORIG] = 0;
    if (fseek(db_file->fpdb, sizeof(struct pictdb_header) + index * sizeof(struct pict_metadata), SEEK_SET) != 0 ||
        fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    }
    
    return 0;
}
