/**
 * @file db_Read.c
 * @implementation of do_read to read an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 29 April 2016
 */

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "pictDB.h"
#include "image_content.h"

int do_read(const char *pict_id, unsigned int res, char image_buffer[], uint32_t *image_size,
            struct pictdb_file *db_file)
{

    if (image_buffer == NULL || pict_id == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    if (res != RES_THUMB && res != RES_SMALL && res != RES_ORIG) {
        return ERR_INVALID_ARGUMENT;
    }

    size_t index = db_file->header.max_files;
    for (size_t i = 0; index == db_file->header.max_files && i < db_file->header.max_files; ++i) {

        if (db_file->metadata[i].is_valid == NON_EMPTY && !strncmp(pict_id, db_file->metadata[i].pict_id, MAX_PIC_ID)) {
            index = i;
        }
    }

    // In case we didn't find the invalid corresponding to the given pict_id
    if (index == db_file->header.max_files) {
        return ERR_FILE_NOT_FOUND;
    }

    // In case the image does not yet exists at the given size
    if (db_file->metadata[index].size[res] == 0) {
        assert(res != RES_ORIG);
        int status = lazy_resize(res, db_file, index);
        if (status != 0) {
            return status;
        }
    }

    uint32_t size = db_file->metadata[index].size[res];
    image_buffer = malloc(size);
    if (image_buffer == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;
    if (fseek(db_file->fpdb, (long) db_file->metadata[index].offset[res], SEEK_SET) != 0 ||
        fread((void *) image_buffer, size, 1, db_file->fpdb) != 1) {
        status = ERR_IO;
    } else {
        *image_size = size;
    }

    return status;
}
