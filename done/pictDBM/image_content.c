/**
 * @file image_content.c
 * @implementation of lazy_image to resize an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 2 April 2016
 */

#include <stdlib.h>
#include <vips/vips.h>

#include "pictDB.h"

int lazy_resize(unsigned int res, struct pictdb_file* file, size_t index)
{
    if (res == RES_ORIG) {
        return 0;
    }

    if (res != RES_THUMB || res != RES_SMALL) {
        return ERR_INVALID_ARGUMENT;
    }

    if (file == NULL) {
        return ERR_FILE_NOT_FOUND;
    }

    if (index > file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    size_t imgSize = file->metadata->size[res];
    long int offset = file->metadata->offset[res];
    void* image = (void*)malloc(imgSize);

    if (image == NULL) {
        return ERR_IO;
    }

    if (fseek(file->fpdb, offset, SEEK_SET) != offset) {
        return ERR_IO;
    }

    if (fread(image, offset, 1, file->fpdb) != 1) {
        return ERR_IO;
    }



    free(image);

    return 0;
}
