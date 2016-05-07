/**
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 9 March 2016
 */

#include "pictDB.h"

/********************************************************************//**
 * List all pictures included in db_file.
 */
const char* do_list(const struct pictdb_file* db_file, enum do_list_mode mode)
{
    if (db_file == NULL || db_file->metadata == NULL) {
        return NULL;
    }

    if (mode != STDOUT && mode != JSON) {
        return "unimplemented do_list mode";
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

    switch (mode) {
        case STDOUT:

            break;
        case JSON:

            break;
    }

    return NULL;
}
