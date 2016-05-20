/**
 * @file db_gbcollect.c
 * @implementation of do_gbcollect to do garbage collecting
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 20 May 2016
 */

#include "pictDB.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    size_t offset;
    size_t size;
} memory_zone;

int cmp_mem(void const *arg1, void const *arg2) {
    memory_zone const *const m1 = arg1;
    memory_zone const *const m2 = arg2;
    return m1->offset > m2->offset;
}

int do_gbcollect(struct pictdb_file *db_file, const char *filename) {
    int status = 0;

    if (db_file->header.num_files > 0) {
        // We insert all memory information of images that are referred in the metadata
        memory_zone zones[NB_RES * db_file->header.max_files];
        size_t idx = 0;
        for (size_t i = 0; i < db_file->header.max_files; ++i) {
            if (db_file->metadata[i].is_valid == NON_EMPTY) {
                size_t offset;
                for (size_t j = 0; j < NB_RES; ++j) {
                    if ((offset = db_file->metadata[i].offset[j]) != 0) {
                        zones[idx++] = (memory_zone) {offset, db_file->metadata[i].size[j]};
                    }
                }
            }
        }
        if (idx == 0) {
            return 0;
        }

        qsort(zones, idx, sizeof(memory_zone), (int (*)(void const *, void const *)) cmp_mem);

        FILE *file = fopen(filename, "r+b");

        if (file == NULL) {
            return ERR_IO;
        }


        for (size_t i = 0; i < idx - 1 && status == 0; ++i) {
            size_t offset = zones[i].offset;
            size_t size = zones[i].size;
            if (offset + size < zones[i + 1].offset) {
                char buf[zones[i].size];
                if (fseek(file, offset, SEEK_SET) != 0
                    || fwrite(memset(buf, 0, size), size, 1, file) != 1) {
                    status = ERR_IO;
                }
            }
        }

        if (fseek(file, 0, SEEK_END) != 0) {
            status = ERR_IO;
        }

        fclose(file);

        if (status == 0 && zones[idx - 1].offset + zones[idx - 1].size < ftell(file)) {
            if (truncate(filename, (off_t) (zones[idx - 1].offset + zones[idx - 1].size)) != 0) {
                status = ERR_IO;
            }
        }

    }
    return status;
}
