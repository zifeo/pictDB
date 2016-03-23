/* ** NOTE: undocumented in Doxygen
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 9 March 2016
 */

#include "pictDB.h"

void do_list (const struct pictdb_file file) {
    print_header(file.header);
    size_t count = 0;
    for (size_t i = 0; i < MAX_MAX_FILES; ++i) {
        //
        if (file.metadata[i].is_valid == NON_EMPTY) {
            print_metadata(file.metadata[i]);
            count += 1;
        }
    }
    if (count == 0) {
        printf("<< empty database >>\n");
    }
}
