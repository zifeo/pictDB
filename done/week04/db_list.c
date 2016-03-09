/* ** NOTE: undocumented in Doxygen
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard
 * @date 9 March 2016
 */

#include "pictDB.h"

#include <stdio.h> // for sprintf

void do_list (const struct pictdb_file file){
    print_header(file.header);
    // empty is set to non zero if there is no valid image (set to 0 once one is valid)
    int empty = 1;
    for (int i=0;i<MAX_MAX_FILES;++i){
        if (!file.metadata[i].is_valid){
            print_metadata(file.metadata[i]);
            empty = 0;
        }
    }
    if (empty){
        printf("<< empty database >>\n");
    }
    
}