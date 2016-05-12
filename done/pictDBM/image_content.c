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
static inline double resize_ratio(unsigned int current_width, unsigned int current_height, unsigned int max_goal_width,
                                  unsigned int max_goal_height)
{
    double h_shrink = (double) max_goal_width / (double) current_width;
    double v_shrink = (double) max_goal_height / (double) current_height;
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

    M_REQUIRE_NON_NULL(db_file);

    if ((res != RES_THUMB && res != RES_SMALL) || index >= db_file->header.max_files) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    if (db_file->metadata[index].is_valid == EMPTY) {
        return ERR_INVALID_PICID;
    }

    // If the image_in already exists just returns
    if (db_file->metadata[index].offset[res] != 0) {
        return 0;
    }

    size_t image_size = db_file->metadata[index].size[RES_ORIG];

    void *image_in = malloc(image_size);
    if (image_in == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    int status = 0;

    if (fseek(db_file->fpdb, (long) db_file->metadata[index].offset[RES_ORIG], SEEK_SET) != 0 ||
        fread(image_in, image_size, 1, db_file->fpdb) != 1) {

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
        void *image_out = NULL;

        if (vips_jpegload_buffer(image_in, image_size, vips_in_image, NULL) != 0 ||
            vips_resize(*vips_in_image, vips_out_image, ratio, NULL) != 0 ||
            vips_jpegsave_buffer(*vips_out_image, &image_out, &res_len, NULL) != 0) {

            status = ERR_VIPS;

        } else if (fseek(db_file->fpdb, 0, SEEK_END) != 0 ||
                   (end_offset = ftell(db_file->fpdb)) == -1 ||
                   fwrite(image_out, res_len, 1, db_file->fpdb) != 1) {

            status = ERR_IO;

        } else {

            db_file->metadata[index].offset[res] = (uint64_t) end_offset;
            db_file->metadata[index].size[res] = (uint32_t) res_len;

            if (fseek(db_file->fpdb, 0, SEEK_SET) != 0 ||
                fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {

                status = ERR_IO;

                // The metadata is after the header and then at the position idx * size(metadata)
                // Note that we start at SEEK_CUR to avoid adding the size of the header
            } else if (fseek(db_file->fpdb, (long) (index * sizeof(struct pict_metadata)), SEEK_CUR) != 0 ||
                       fwrite(&db_file->metadata[index], sizeof(struct pict_metadata), 1, db_file->fpdb) != 1) {

                status = ERR_IO;

            };
        }

        g_free(image_out);
        image_out = NULL;

        g_object_unref(process);
        process = NULL;
    }

    free(image_in);
    image_in = NULL;
    return status;
}

/********************************************************************//**
 * Returns resolution from image given an image.
 */
int get_resolution(uint32_t *height, uint32_t *width, const char *image_buffer, size_t image_size)
{
    M_REQUIRE_NON_NULL(image_buffer);

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
    process = NULL;

    return status;
}
