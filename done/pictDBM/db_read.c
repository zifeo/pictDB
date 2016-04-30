/**
 * @file db_Read.c
 * @implementation of do_read to read an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 29 April 2016
 */

#include <string.h>
#include <stdlib.h>
#include "pictDB.h"
#include "image_content.h"

int readImageData(size_t index, unsigned int res, const char **image_buffer, uint32_t *image_size,
                  struct pictdb_file *db_file)
{
    int status = 0;

    uint32_t size = db_file->metadata[index].size[res];

    *image_buffer = malloc(size);

    if (*image_buffer == NULL) {
        status = ERR_OUT_OF_MEMORY;
    } else {
        if (fseek(db_file->fpdb, (long) db_file->metadata[index].offset[res], SEEK_SET) != 0 ||
            fread((void *) *image_buffer, size, 1, db_file->fpdb) != 1) {
            status = ERR_IO;
        } else {
            *image_size = size;
        }
    }

    //free(buffer);
    //buffer = NULL;
    return status;
}

int do_read(const char *pict_id, unsigned int res, const char **image_buffer, uint32_t *image_size,
            struct pictdb_file *db_file)
{
    if (image_buffer == NULL || pict_id == NULL || db_file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    size_t index = 0;

    for (size_t i = 0; index == 0 && i < db_file->header.max_files; ++i) {
        if (db_file->metadata[i].is_valid == EMPTY && !strcmp(pict_id,
                db_file->metadata[i].pict_id)) {
            index = i;
        }
    }

    // In case we didn't find the invalid corresponding to the given pict_id
    if (index == 0) {
        return ERR_FILE_NOT_FOUND;
    }

    int status = 0;

    // In case the image already exists at the given size
    if (db_file->metadata[index].size[res] == 0) {
        return readImageData(index, res, image_buffer, image_size, db_file);
    }

    if ((status = lazy_resize(res, db_file, index)) != 0) {
        return status;
    }

    // This is a success we can just copy now the value written by lazy resize in our metadata
    return readImageData(index, res, image_buffer, image_size, db_file);
}