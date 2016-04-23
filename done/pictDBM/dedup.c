/**
 * @file dedup.c
 * @implementation of do_name_and_content_dedup to avoid storing the same image multiple times
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 23 April 2016
 */

#include "dedup.h"
#include <string.h>
#include <stdlib.h>

int SHA_equals(unsigned char SHA1[], unsigned char SHA2[]) {
    int i = 0;
    for (i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        if (SHA1[i] != SHA2[i]) {
            return 1;
        }
    }
    return 0;
}

int delete_content_file(FILE *fpdb, size_t no_bytes, size_t offset) {

    void *empty = malloc(no_bytes);
    if (empty == NULL) {
        return ERR_OUT_OF_MEMORY;
    }
    int status = 0;

    memset(empty, 0, no_bytes);

    if (fseek(fpdb, offset, SEEK_SET) != 0 || fwrite(empty, no_bytes, 1, fpdb) != 1) {
        status = ERR_IO;
    }

    free(empty);
    empty = NULL;
    return status;
}

int do_name_and_dedup(struct pictdb_file *db_file, uint32_t index) {

    int i = 0;

    if (db_file->metadata[index].is_valid == EMPTY) {
        // TODO what error to return ?
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    for (i = 0; i < db_file->header.max_files; ++i) {
        if (i != index && db_file->metadata[i].is_valid == NON_EMPTY) {
            if (!strcmp(db_file->metadata[i].pict_id, db_file->metadata[index].pict_id)) {
                return ERR_DUPLICATE_ID;

            } else if (SHA_equals(db_file->metadata[i].SHA, db_file->metadata[index].SHA)) {

                uint64_t thumb_offset = db_file->metadata[index].offset[RES_THUMB];
                uint64_t small_offset = db_file->metadata[index].offset[RES_SMALL];
                uint64_t orig_offset = db_file->metadata[index].offset[RES_ORIG];

                uint64_t thumb_size = db_file->metadata[index].size[RES_THUMB];
                uint64_t small_size = db_file->metadata[index].size[RES_SMALL];
                uint64_t orig_size = db_file->metadata[index].size[RES_ORIG];

                db_file->metadata[index].offset[RES_THUMB] = db_file->metadata[i].offset[RES_THUMB];
                db_file->metadata[index].offset[RES_SMALL] = db_file->metadata[i].offset[RES_SMALL];
                db_file->metadata[index].offset[RES_ORIG] = db_file->metadata[i].offset[RES_ORIG];
                db_file->metadata[index].size[RES_THUMB] = db_file->metadata[i].size[RES_THUMB];
                db_file->metadata[index].size[RES_SMALL] = db_file->metadata[i].size[RES_SMALL];

                if (fseek(db_file->fpdb, sizeof(struct pictdb_header) + index * sizeof(struct pict_metadata),
                          SEEK_SET) != 0 ||
                    fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
                    return ERR_IO;
                }

                // Now we set all the old values (e.g. values at the previous offsets) to zero to save space
                if (delete_content_file(db_file->fpdb, thumb_size, thumb_offset) ||
                    delete_content_file(db_file->fpdb, small_size, small_offset) ||
                    delete_content_file(db_file->fpdb, orig_size, orig_offset)) {
                    return ERR_IO;
                }
                return 0;
            }
        }
    }

    db_file->metadata[index].offset[RES_ORIG] = 0;
    if (fseek(db_file->fpdb, sizeof(struct pictdb_header) + index * sizeof(struct pict_metadata),
              SEEK_SET) != 0 ||
        fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    }
    return 0;
}
