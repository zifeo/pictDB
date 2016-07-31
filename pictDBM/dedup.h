/**
 * @file dedup.h
 * @implementation of do_name_and_content_dedup to avoid storing the same image multiple times
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 23 April 2016
 */

#ifndef PICTDBPRJ_DEDUP_H
#define PICTDBPRJ_DEDUP_H

#include "pictDB.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check for duplicates in the file and de-duplicate image at given index if present
 *
 * @param db_file In memory structure with header and metadata.
 * @param index The metadata index of the image
 */
int do_name_and_content_dedup(struct pictdb_file *db_file, const uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
