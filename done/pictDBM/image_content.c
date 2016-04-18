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

// ======================================================================
/**
 * @brief Computes the resize factor by keeping aspect ratio
 *
 * @param current_width The current width.
 * @param current_height The current height.
 * @param max_goal_width The maximum goal width.
 * @param max_goal_height The maximum goal height.
 */
double resize_ratio(int current_width, int current_height, int max_goal_width, int max_goal_height)
{
    const double h_shrink = (double) current_width  / (double) max_goal_width ;
    const double v_shrink = (double) current_height / (double) max_goal_height ;
    return h_shrink > v_shrink ? v_shrink : h_shrink ;
}

int lazy_resize(unsigned int res, struct pictdb_file *file, size_t index)
{
    if (res == RES_ORIG) {
        return 0;
    }

    if ((res != RES_THUMB && res != RES_SMALL) || file == NULL || index > file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    if (file->fpdb == NULL) {
        return ERR_IO;
    }

    if (index > file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    // If the image already exists just returns
    if (file->metadata[index].is_valid == NON_EMPTY) {
        return 0;
    }

    size_t image_size = file->metadata[index].size[res];
    size_t offset = file->metadata[index].offset[res];
    void* image = malloc(image_size);

    if (image == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;

    if (fseek(file->fpdb, offset, SEEK_SET) != 0 ||
        fread(image, image_size, 1, file->fpdb) != 1) {
        status = ERR_IO;
    } else {

        VipsObject* process = VIPS_OBJECT(vips_image_new());
        double ratio = resize_ratio(file->header.res_resized[RES_ORIG], file->header.res_resized[RES_ORIG + 1],
                                    file->header.res_resized[res], file->header.res_resized[res]);

        VipsImage** vips_in_image = (VipsImage **) vips_object_local_array(process, 1);
        VipsImage** vips_out_image = (VipsImage **) vips_object_local_array(process, 1);

        if (vips_jpegload_buffer(image, image_size, vips_in_image) != 0 ||
            vips_resize(*vips_in_image, vips_out_image, ratio) != 0 ||
            vips_jpegsave_buffer(*vips_out_image, image, &image_size) != 0) {

            status = ERR_VIPS;
        } else if (fseek(file->fpdb, 0, SEEK_END) != 0 ||
                   fwrite(vips_out_image, sizeof(image), 1, file->fpdb) != 1) {
            return ERR_IO;
        }

        g_object_unref(process);
    }

    free(image);
    image = NULL;

    return status;
}
