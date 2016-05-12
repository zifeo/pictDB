/**
 * @file db_list.c
 * @implementation of do_list to display the content of a file
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 9 March 2016
 */

#include <stdlib.h>
#include "pictDB.h"
#include <json-c/json.h>

#define UNKNOWN_MODE "unimplemented do_list mode"
#define PICS_JSON_LABEL "Pictures"

/********************************************************************//**
 * List all pictures included in db_file.
 */
const char* do_list(const struct pictdb_file* db_file, enum do_list_mode mode)
{

    if (db_file == NULL || db_file->metadata == NULL) {
        return NULL;
    }

    if (mode != STDOUT && mode != JSON) {
        // TODO : what to do here ?
        return UNKNOWN_MODE;
    }

    switch (mode) {
    case STDOUT: {

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

        return NULL;
    }
    case JSON: {

        struct json_object *obj = json_object_new_object();
        struct json_object *arr = json_object_new_array();

        for (size_t i = 0; i < db_file->header.max_files; ++i) {
            if (db_file->metadata[i].is_valid == NON_EMPTY) {

                struct json_object* id = json_object_new_string(db_file->metadata[i].pict_id);
                // TODO : return code ?
                json_object_array_add(arr, id);
            }
        }

        json_object_object_add(obj, PICS_JSON_LABEL, arr);
        const char* json = json_object_to_json_string(obj);
        json_object_put(obj);

        return json;
    }
    }
}
