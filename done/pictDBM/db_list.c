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
#include <assert.h>
#include <string.h>

#define PICS_JSON_LABEL "Pictures"

static const char unknown_mode[] = "unimplemented do_list mode";

/********************************************************************//**
 * List all pictures included in db_file.
 * Always return null or an mallocated string.
 */
char* do_list(const struct pictdb_file* db_file, enum do_list_mode mode)
{

    if (db_file == NULL || db_file->metadata == NULL) {
        return NULL;
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

                struct json_object *id = json_object_new_string(db_file->metadata[i].pict_id);
                int status = json_object_array_add(arr, id);
                assert(status == 0);
            }
        }

        json_object_object_add(obj, PICS_JSON_LABEL, arr);
        const char *json = json_object_to_json_string(obj);

        size_t ret_size = strlen(json);
        char *ret = malloc(ret_size + 1);
        strncpy(ret, json, ret_size);
        ret[ret_size] = '\0';

        int status = json_object_put(obj);
        assert(status == 1);

        return ret;
    }
    default: {
        size_t ret_size = strlen(unknown_mode);
        char *ret = malloc(ret_size + 1);
        strncpy(ret, unknown_mode, ret_size);
        ret[ret_size] = '\0';
        return ret;
    }
    }
}
