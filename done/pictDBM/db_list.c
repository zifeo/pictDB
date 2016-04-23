/**
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 9 March 2016
 */

#include "pictDB.h"

int do_list(const struct pictdb_file* db_file)
{
    if (db_file == NULL || db_file->metadata == NULL) {
        return ERR_INVALID_ARGUMENT;
    }
    print_header(&db_file->header);

    if (db_file->header.num_files > 0) {
        for (size_t i = 0; i < db_file->header.max_files; ++i) {
            if (db_file->metadata[i].is_valid == NON_EMPTY) {
                print_metadata(&db_file->metadata[i]);
            }
        }
    } else {
        printf("<< empty database >>\n");
    }

    return 0;
}
