/**
 * @file db_gbcollect.c
 * @implementation of do_gbcollect to do garbage collecting
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 20 May 2016
 */

#include "pictDB.h"
#include "image_content.h"

int db_contains_holes(const struct pictdb_file *db_file) {
    uint32_t num_files = db_file->header.num_files;
    if (num_files > 0) {
        for (size_t i = 0; i < db_file->header.max_files; ++i) {
            if (db_file->metadata[i].is_valid == EMPTY) {
                return 1;
            }
        }
    }
    return 0;
}

int do_gbcollect(const struct pictdb_file *db_file, const char *db_filename, const char *tmp_db_filename) {

    M_REQUIRE_NON_NULL(db_file);
    M_REQUIRE_NON_NULL(db_filename);
    M_REQUIRE_NON_NULL(tmp_db_filename);

    /*
    if (!db_contains_holes(db_file)) {
        return 0;
    }
    */
    struct pictdb_file tmp_db_file;

    // copy the optional argument of the header (the ones usually specified at creation)
    tmp_db_file.header.max_files = db_file->header.max_files;
    tmp_db_file.header.res_resized[RES_THUMB] = db_file->header.res_resized[RES_THUMB];
    tmp_db_file.header.res_resized[RES_THUMB + 1] = db_file->header.res_resized[RES_THUMB + 1];
    tmp_db_file.header.res_resized[RES_SMALL << 1] = db_file->header.res_resized[RES_THUMB << 1];
    tmp_db_file.header.res_resized[(RES_SMALL << 1) + 1] = db_file->header.res_resized[(RES_THUMB << 1) + 1];

    int status = do_create(tmp_db_filename, &tmp_db_file);

    if (status == 0) {
        status = do_open(tmp_db_filename, "r+b", &tmp_db_file);

        if (status == 0) {

            if (db_file->header.num_files > 0) {
                for (size_t i = 0; i < db_file->header.max_files && status == 0; ++i) {
                    if (db_file->metadata[i].is_valid == NON_EMPTY) {
                        size_t offset, size;
                        for (unsigned int res = 0; res < NB_RES; ++res) {
                            if ((offset = db_file->metadata[i].offset[res]) != 0 &&
                                (size = db_file->metadata[i].size[res]) != 0) {
                                if (res == RES_ORIG) {
                                    char buffer[size];
                                    if (fseek(db_file->fpdb, offset, SEEK_SET) != 0 ||
                                        fread(buffer, size, 1, db_file->fpdb) != 1) {
                                        status = ERR_IO;
                                    }
                                    status = do_insert(buffer, size, db_file->metadata[i].pict_id, &tmp_db_file);
                                } else {
                                    status = lazy_resize(res, &tmp_db_file, i);
                                };
                            }
                        }
                    }
                }
            }
            // Now remove and rename the tmp file in case of success
            if (status == 0) {
                status = fclose(db_file->fpdb) != 0 ? ERR_IO : 0;

                if (status == 0) {
                    status = remove(db_filename);

                    if (status == 0) {
                        status = rename(tmp_db_filename, db_filename);
                    }
                }
            }
            do_close(&tmp_db_file);
        }
    }

    return status;
}
