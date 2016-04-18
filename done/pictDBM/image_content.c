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
double resize_ratio(int current_width, int current_height, int max_goal_width, int max_goal_height) {
    const double h_shrink = (double) current_width / (double) max_goal_width;
    const double v_shrink = (double) current_height / (double) max_goal_height;
    return h_shrink > v_shrink ? v_shrink : h_shrink;
}

int lazy_resize(unsigned int res, struct pictdb_file *db_file, size_t index) {
    printf("HERE");
    if (res == RES_ORIG) {
        return 0;
    }

    if ((res != RES_THUMB && res != RES_SMALL) || db_file == NULL || index > db_file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    if (index > db_file->header.num_files) {
        return ERR_INVALID_ARGUMENT;
    }



    // If the image already exists just returns
    if (db_file->metadata[index].is_valid == NON_EMPTY &&
        db_file->metadata[index].offset[res] != 0) {
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
        double ratio = resize_ratio(db_file->header.res_resized[RES_ORIG],
                                    db_file->header.res_resized[RES_ORIG + 1],
                                    db_file->header.res_resized[res], db_file->header.res_resized[res]);

        VipsImage **vips_in_image = (VipsImage **) vips_object_local_array(process, 1);
        VipsImage **vips_out_image = (VipsImage **) vips_object_local_array(process, 1);


        long endOffset;

        if (vips_jpegload_buffer(image, image_size, vips_in_image, NULL) != 0 ||
            vips_resize(*vips_in_image, vips_out_image, ratio, NULL) != 0 ||
            vips_jpegsave_buffer(*vips_out_image, &image, &image_size, NULL) != 0) {

            status = ERR_VIPS;
        } else if (fseek(db_file->fpdb, 0, SEEK_END) != 0 ||
                   (endOffset = ftell(db_file->fpdb)) == 1L ||
                   fwrite(vips_out_image, sizeof(image), 1, db_file->fpdb) != 1) {
            status = ERR_IO;
        } else {
            db_file->header.db_version += 1;
            db_file->metadata[index].offset[res] = (uint64_t) endOffset;
            if (fseek(db_file->fpdb, 0, SEEK_SET) != 0 ||
                fwrite(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {
                status = ERR_IO;
            }
        }

        //g_object_unref(process);
    }

    //free(image);
    //image = NULL;
    return 0;
}
