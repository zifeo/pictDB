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
#define PIC_JSON_ID "id"
#define PIC_JSON_RES "res"

#define HEADER_JSON_LABEL "Header"
#define DB_JSON_DB_NAME "db_name"
#define DB_JSON_VERSION "db_version"
#define DB_JSON_NUM_FILES "num_files"
#define DB_JSON_MAX_FILES "max_files"

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
                struct json_object *img = json_object_new_object();
                struct json_object *id = json_object_new_string(db_file->metadata[i].pict_id);
                char s_size[16];
                sprintf(s_size, "%" PRIu32 " x %" PRIu32, db_file->metadata->res_orig[0], db_file->metadata->res_orig[1]);
                struct json_object *size = json_object_new_string(s_size);
                json_object_object_add(img, PIC_JSON_ID, id);
                json_object_object_add(img, PIC_JSON_RES, size);
                int status = json_object_array_add(arr, img);
                assert(status == 0);
            }
        }

        json_object_object_add(obj, PICS_JSON_LABEL, arr);

        struct json_object *header = json_object_new_object();

        struct json_object *db_name = json_object_new_string(db_file->header.db_name);
        json_object_object_add(header, DB_JSON_DB_NAME, db_name);

        char s_num_files[16];
        sprintf(s_num_files, "%" PRIu32, db_file->header.num_files);
        struct json_object *num_files = json_object_new_string(s_num_files);
        json_object_object_add(header, DB_JSON_NUM_FILES, num_files);

        char s_max_files[16];
        sprintf(s_max_files, "%" PRIu32, db_file->header.max_files);
        struct json_object *max_files = json_object_new_string(s_max_files);
        json_object_object_add(header, DB_JSON_MAX_FILES, max_files);

        char s_version[16];
        sprintf(s_version, "%" PRIu32, db_file->header.db_version);
        struct json_object *db_Version = json_object_new_string(s_version);
        json_object_object_add(header, DB_JSON_VERSION, db_Version);

        json_object_object_add(obj, HEADER_JSON_LABEL, header);

        const char *json = json_object_to_json_string(obj);

        printf("\n%s\n", json);

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
