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

// ======================================================================
/**
 * @brief Check whether or not the two given SHAs are equals
 *
 * @param SHA1 The first SHA
 * @param SHA2 The second SHA
 */
int SHA_equals(unsigned char SHA1[], unsigned char SHA2[]);

// ======================================================================
/**
 * @brief Check for duplicates in the file and de-duplicate image at given index if present
 *
 * @param db_file In memory structure with header and metadata.
 * @param index The metadata index of the image
 */
int do_name_and_dedup(struct pictdb_file *db_file, uint32_t index);

#endif
