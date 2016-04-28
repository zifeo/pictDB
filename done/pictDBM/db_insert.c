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

int do_insert(const char **image_buffer, size_t image_size, const char *pict_id, struct pictdb_file *db_file) {
    if (image_buffer == NULL || pict_id == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    // We assume the file is already opened from the outside so we don't do it here

    // 1) Find a free position at the index
    // In fact the equality is strict but to ensure not writting
    if (db_file->header.num_files >= db_file->header.max_files) {
        return ERR_FULL_DATABASE;
    }

    size_t index = 0;

    for (size_t i = 0; index == 0 && i < db_file->header.max_files; ++i) {
        if (db_file->metadata[i].is_valid == EMPTY) {
            index = i;
            db_file->metadata[i].is_valid = NON_EMPTY;

            unsigned char *SHA = malloc(SHA_DIGEST_LENGTH);
            SHA256((const unsigned char *) pict_id, SHA_DIGEST_LENGTH, SHA);
            strncpy(db_file->metadata[i].SHA, SHA, SHA_DIGEST_LENGTH);

            strncpy(db_file->metadata[i].pict_id, pict_id, MAX_PIC_ID + 1);

            db_file->metadata[i].size[RES_ORIG] = (uint32_t) image_size;
        }
    }

    int status = 0;

    // 2) Image de-duplication
    if ((status = do_name_and_dedup(db_file, (const uint32_t) index)) != 0) {
        return status;
    }

    // 3) Write image on disk
    // If the image already existed simply exits
    if (db_file->metadata[index].offset[RES_ORIG] == 0) {
        return 0;
    }

    long end_offset = 0;
    if (fseek(db_file->fpdb, 0, SEEK_END) != 0 ||
        (end_offset = ftell(db_file->fpdb)) == -1 ||
        fwrite(image_buffer, image_size, 1, db_file->fpdb) != 1) {
        return ERR_IO;
    }

    db_file->metadata[index].offset[RES_ORIG] = (uint64_t) end_offset;

    // 4) Update database
    db_file->header.db_version += 1;
    db_file->header.num_files += 1;

    //TODO ask for max size 512x512
    if ((status = get_resolution(&db_file->metadata[index].res_orig[0], &db_file->metadata[index].res_orig[1],
                                 *image_buffer, image_size)) != 0) {
        return status;
    }

    if (fseek(db_file->fpdb, 0, SEEK_SET) != 0 ||
        fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    }

    if (fseek(db_file->fpdb, index * sizeof(struct pict_metadata), SEEK_CUR) != 0 ||
        fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {
        return ERR_IO;
    };

    return status;
}