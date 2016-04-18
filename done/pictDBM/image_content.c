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

int lazy_resize(unsigned int res, struct pictdb_file *file, size_t index) {
    if (res == RES_ORIG) {
        return 0;
    }

    if ((res != RES_THUMB && res != RES_SMALL) ||
        file == NULL ||
        index > file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    if (file->fpdb == NULL) {
        return ERR_IO;
    }

    if (index > file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    // If the image already exists just returns
    if (file->metadata[index].is_valid == EMPTY) {
        return 0;
    }

    size_t image_size = file->metadata[index].size[res];
    size_t offset = file->metadata[index].offset[res];
    void *image = malloc(image_size);

    if (image == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    if (fseek(file->fpdb, offset, SEEK_SET) != 0) {
        return ERR_IO;
    }

    if (fread(image, offset, 1, file->fpdb) != 1) {
        return ERR_IO;
    }

    VipsObject *process = VIPS_OBJECT(vips_image_new());

    // we want 1 new image
    VipsImage **vips_in_image = (VipsImage **) vips_object_local_array(process, 1);
    VipsImage **vips_out_image = (VipsImage **) vips_object_local_array(process, 1);

    if (vips_jpegload_buffer(image, image_size, vips_in_image) != 0) {
        // error
    }

    double ratio = file->header.res_resized[res] / (double) file->header.res_resized[RES_ORIG];

    if (vips_resize(*vips_in_image, vips_out_image, ratio) != 0) {

    }

    if (fseek(file->fpdb, 0, SEEK_END) != 0) {

    }

    if (vips_jpegsave_buffer(*vips_out_image, NULL, sizeof(image)) != 0) {

    }

    if (fwrite(vips_out_image, sizeof(image), 1, file->fpdb) != 1) {

    }

    free(image);
    image = NULL;

    g_object_unref(process);

    return 0;
}
