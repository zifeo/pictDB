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

/********************************************************************//**
 * Compute the aspect ratio from given sizes.
 */
double resize_ratio(int current_width, int current_height, int max_goal_width, int max_goal_height)
{
    const double h_shrink = (double) max_goal_width / (double) current_width;
    const double v_shrink = (double) max_goal_height / (double) current_height;
    return h_shrink > v_shrink ? v_shrink : h_shrink;
}

/********************************************************************//**
 * Resize given picture in given resolution on the need.
 */
int lazy_resize(unsigned int res, struct pictdb_file *db_file, size_t index)
{
    if (res == RES_ORIG) {
        return 0;
    }

    if ((res != RES_THUMB && res != RES_SMALL) || db_file == NULL || index > db_file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    if (db_file->metadata[index].is_valid == EMPTY) {
        return ERR_INVALID_PICID;
    }

    // If the image already exists just returns
    if (db_file->metadata[index].offset[res] != 0) {
        return 0;
    }

    size_t image_size = db_file->metadata[index].size[RES_ORIG];

    void *image = malloc(image_size);

    if (image == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;

    if (fseek(db_file->fpdb, (long) db_file->metadata[index].offset[RES_ORIG], SEEK_SET) != 0 ||
        fread(image, image_size, 1, db_file->fpdb) != 1) {

        status = ERR_IO;

    } else {

        VipsObject *process = VIPS_OBJECT(vips_image_new());
        double ratio = resize_ratio(db_file->metadata[index].res_orig[0],
                                    db_file->metadata[index].res_orig[1],
                                    db_file->header.res_resized[2 * res],
                                    db_file->header.res_resized[2 * res + 1]);

        VipsImage **vips_in_image = (VipsImage **) vips_object_local_array(process, 1);
        VipsImage **vips_out_image = (VipsImage **) vips_object_local_array(process, 1);

        long end_offset = 0;
        size_t res_len = 0;

        // We do not need to check if the new resolution is less than the
        // original since we are always going to reduce the size of the image
        if (vips_jpegload_buffer(image, image_size, vips_in_image, NULL) != 0 ||
            vips_resize(*vips_in_image, vips_out_image, ratio, NULL) != 0 ||
            vips_jpegsave_buffer(*vips_out_image, &image, &res_len, NULL) != 0) {

            status = ERR_VIPS;

        } else if (fseek(db_file->fpdb, 0, SEEK_END) != 0 ||
                   (end_offset = ftell(db_file->fpdb)) == -1 ||
                   fwrite(image, res_len, 1, db_file->fpdb) != 1) {

            status = ERR_IO;

        } else {

            db_file->metadata[index].offset[res] = (uint64_t) end_offset;
            db_file->metadata[index].size[res] = (uint32_t) res_len;

            if (fseek(db_file->fpdb, 0, SEEK_SET) != 0 ||
                fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {

                status = ERR_IO;

                // The metadata is after the header and then at the position idx * size(metadata)
                // Note that we start at SEEK_CUR to avoid adding the size of the header
            } else if (fseek(db_file->fpdb, index * sizeof(struct pict_metadata), SEEK_CUR) != 0 ||
                       fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {

                status = ERR_IO;

            };
        }

        g_object_unref(process);
    }

    free(image);
    image = NULL;
    return status;
}

/********************************************************************//**
 * Returns resolution from image given an image.
 */
int get_resolution(uint32_t *height, uint32_t *width, const char *image_buffer, size_t image_size)
{
    if (image_buffer == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    VipsObject *process = VIPS_OBJECT(vips_image_new());
    VipsImage **vips_image = (VipsImage **) vips_object_local_array(process, 1);

    int status = 0;

    if (vips_jpegload_buffer((void *) image_buffer, image_size, vips_image, NULL) != 0) {
        status = ERR_VIPS;
    } else {
        *width = (uint32_t) vips_image_get_width(*vips_image);
        *height = (uint32_t) vips_image_get_height(*vips_image);
    }

    g_object_unref(process);
    return status;
}
