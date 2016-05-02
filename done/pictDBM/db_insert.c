/**
 * @file db_insert.c
 * @implementation of do_insert to insert an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 28 April 2016
 */

#include <string.h>
#include <stdlib.h>
#include "pictDB.h"
#include "dedup.h"
#include "image_content.h"

int do_insert(const char **image_buffer, size_t image_size, const char *pict_id, struct pictdb_file *db_file)
{
    if (image_buffer == NULL || pict_id == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    // We assume the file is already opened from the outside so we don't do it here

    // 1) Find a free position at the index
    if (db_file->header.num_files >= db_file->header.max_files) {
        return ERR_FULL_DATABASE;
    }

    size_t index = db_file->header.max_files;
    for (size_t i = 0; index == db_file->header.max_files && i < db_file->header.max_files; ++i) {
        if (db_file->metadata[i].is_valid == EMPTY) {

            index = i;

            unsigned char *sha = malloc(SHA_DIGEST_LENGTH);
            SHA256((unsigned char *) image_buffer, image_size, sha);

            int status = 0;
            if (strncpy(db_file->metadata[i].SHA, sha, SHA_DIGEST_LENGTH) != 0) {
                // TODO : which error ?
                status = ERR_DEBUG;
            } else {
                // TODO : max pic id + 1 ?
                if (strncpy(db_file->metadata[i].pict_id, pict_id, MAX_PIC_ID + 1) != 0) {
                    status = ERR_DEBUG;
                } else {
                    db_file->metadata[i].size[RES_ORIG] = (uint32_t) image_size;
                }
            }
            free(sha);
            sha = NULL;

            if (status != 0) {
                return status;
            }
        }
    }

    // 2) Image de-duplication
    int status = do_name_and_content_dedup(db_file, (const uint32_t) index);
    if (status != 0) {
        return status;
    }

    // 3) Write image on disk
    if (db_file->metadata[index].offset[RES_ORIG] == 0) {
        // only if no duplicate
        // fstat is not standard, so fseek + ftell will do the job
        if (fseek(db_file->fpdb, 0, SEEK_END) != 0) {
            return ERR_IO;
        }
        long end_offset = ftell(db_file->fpdb);
        if (end_offset == -1 ||
            fwrite(image_buffer, image_size, 1, db_file->fpdb) != 1) {
            return ERR_IO;
        }

        db_file->metadata[index].offset[RES_THUMB] = 0;
        db_file->metadata[index].offset[RES_SMALL] = 0;
        db_file->metadata[index].offset[RES_ORIG] = (uint64_t) end_offset;
        db_file->metadata[index].size[RES_THUMB] = 0;
        db_file->metadata[index].size[RES_SMALL] = 0;
        db_file->metadata[index].size[RES_ORIG] = (uint32_t) image_size;
    }

    db_file->metadata[index].is_valid = NON_EMPTY;
    db_file->metadata[index].unused_16 = 0;
    status = get_resolution(&db_file->metadata[index].res_orig[0], &db_file->metadata[index].res_orig[1], *image_buffer,
                            image_size);
    if (status != 0) {
        return status;
    }

    // 4) Update database
    db_file->header.db_version += 1;
    db_file->header.num_files += 1;

    if (fseek(db_file->fpdb, 0, SEEK_SET) != 0 ||
        fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1 ||
        fseek(db_file->fpdb, index * sizeof(struct pict_metadata), SEEK_CUR) != 0 ||
        fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    };

    return status;
}