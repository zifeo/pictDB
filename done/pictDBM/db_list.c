/**
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 9 March 2016
 */

#include "pictDB.h"

int do_list (const struct pictdb_file* file)
{

    if (file == NULL) {
        return ERR_INVALID_ARGUMENT;
    }

    print_header(&file->header);

    if (file->header.num_files > 0) {
        for (size_t i = 0; i < MAX_MAX_FILES; ++i) {
            if (file->metadata[i].is_valid == NON_EMPTY) {
                print_metadata(&file->metadata[i]);
            }
        }
    } else {
        printf("<< empty database >>\n");
    }

    return 0;
}
