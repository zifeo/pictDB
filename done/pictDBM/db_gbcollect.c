/**
 * @file db_gbcollect.c
 * @implementation of do_gbcollect to do garbage collecting
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 20 May 2016
 */

#include "pictDB.h"
#include "image_content.h"

int do_gbcollect(const struct pictdb_file *db_file, struct pictdb_file *tmp_db_file) {

    M_REQUIRE_NON_NULL(tmp_db_file);

    if (db_file->header.num_files <= 0) {
        return 0;
    }

    int status = 0;
    for (size_t i = 0; i < db_file->header.max_files && status == 0; ++i) {
        if (db_file->metadata[i].is_valid == NON_EMPTY) {
            size_t offset, size;
            for (unsigned int res = 0; res < NB_RES; ++res) {
                printf("HERE status=%d i=%lu\n", status, i);
                if ((offset = db_file->metadata[i].offset[res]) != 0 &&
                    (size = db_file->metadata[i].size[res]) != 0) {
                    if (res == RES_ORIG) {
                        char buffer[size];
                        if (fseek(db_file->fpdb, offset, SEEK_SET) != 0 || fread(buffer, size, 1, db_file->fpdb) != 1) {
                            status = ERR_IO;
                        }
                        status = do_insert(buffer, size, db_file->metadata[i].pict_id, tmp_db_file);
                    } else {
                        status = lazy_resize(res, tmp_db_file, i);
                    };
                }
            }
        }
    }
    return status;
}
