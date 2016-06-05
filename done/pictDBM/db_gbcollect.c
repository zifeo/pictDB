/**
 * @file db_gbcollect.c
 * @implementation of do_gbcollect to do garbage collecting
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 20 May 2016
 */

#include <assert.h>
#include "pictDB.h"
#include "image_content.h"

int do_gbcollect(const struct pictdb_file *db_file, const char *db_filename, const char *tmp_db_filename)
{
    M_REQUIRE_NON_NULL(db_file);
    M_REQUIRE_NON_NULL(db_filename);
    M_REQUIRE_NON_NULL(tmp_db_filename);

    uint32_t modification_count = 0;
    struct pictdb_file tmp_db_file;

    // copy the optional argument of the header (the ones usually specified at creation)
    tmp_db_file.header.max_files = db_file->header.max_files;
    tmp_db_file.header.res_resized[RES_THUMB] = db_file->header.res_resized[RES_THUMB];
    tmp_db_file.header.res_resized[RES_THUMB + 1] = db_file->header.res_resized[RES_THUMB + 1];
    tmp_db_file.header.res_resized[RES_SMALL << 1] = db_file->header.res_resized[RES_THUMB << 1];
    tmp_db_file.header.res_resized[(RES_SMALL << 1) + 1] = db_file->header.res_resized[(RES_THUMB << 1) + 1];

    int status = do_create(tmp_db_filename, &tmp_db_file);
    if (status != 0) {
        return status;
    }

    if (db_file->header.num_files > 0) {
        for (size_t i = 0; i < db_file->header.max_files && status == 0; ++i) {
            if (db_file->metadata[i].is_valid == NON_EMPTY) {

                // starts by RES_ORIG to insert it and then resize, otherwise resizing may failed
                // as the original image is not yet there
                for (int res = RES_ORIG; res >= 0; --res) {

                    uint64_t offset = db_file->metadata[i].offset[res];
                    uint32_t size = 0;

                    if (offset != 0) {
                        modification_count += 1;

                        if (res == RES_ORIG) {
                            char *image_buffer = NULL;
                            status = do_read(db_file->metadata[i].pict_id, (unsigned int) res, &image_buffer, &size, db_file);

                            if (status == 0) {
                                assert(image_buffer != NULL);
                                status = do_insert(image_buffer, size, db_file->metadata[i].pict_id, &tmp_db_file);
                            }

                            if (image_buffer != NULL) {
                                free(image_buffer);
                                image_buffer = NULL;
                            }

                        } else {
                            status = lazy_resize((unsigned int) res, &tmp_db_file, i);
                        }
                    }
                }
            }
        }
    }

    do_close(&tmp_db_file);

    // Now remove and rename the tmp file in case of success
    if (status == 0) {

        if (fclose(db_file->fpdb) != 0) {
        } else if (modification_count == 0) {
            status = remove(tmp_db_filename);
        } else {
            status = remove(db_filename);
            if (status == 0) {
                status = rename(tmp_db_filename, db_filename);
            }
        }
    }

    return status;
}
