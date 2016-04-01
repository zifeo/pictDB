/**
 * @file pictDB.h
 * @brief Main header file for pictDB core library.
 *
 * Defines the format of the data structures that will be stored on the disk
 * and provides interface functions.
 *
 * The picture database starts with exactly one header structure
 * followed by exactly pictdb_header.max_files metadata
 * structures. The actual content is not defined by these structures
 * because it should be stored as raw bytes appended at the end of the
 * database file and addressed by offsets in the metadata structure.
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#ifndef PICTDBPRJ_PICTDB_H
#define PICTDBPRJ_PICTDB_H

#include "error.h" /* not needed here, but provides it as required by
                    * all functions of this lib.
                    */
#include <stdio.h> // for FILE
#include <stdint.h> // for uint32_t, uint64_t
#include <openssl/sha.h> // for SHA256_DIGEST_LENGTH

#define CAT_TXT "EPFL PictDB binary"

/* constraints */
#define MAX_DB_NAME 31  // max. size of a PictDB name
#define MAX_PIC_ID 127  // max. size of a picture id
#define MAX_MAX_FILES 10  // will be increased later in the project

/* For is_valid in pictdb_metadata */
#define EMPTY 0
#define NON_EMPTY 1

// pictDB library internal codes for different picture resolutions.
#define RES_THUMB 0
#define RES_SMALL 1
#define RES_ORIG  2
#define NB_RES    3

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Store database general information.
 */
struct pictdb_header {
    char db_name[MAX_DB_NAME + 1]; /**< database name */
    uint32_t db_version; /**< database version */
    uint32_t num_files; /**< database image count */
    uint32_t max_files; /**< database max image count (constant) */
    uint16_t res_resized[2 * (NB_RES - 1)]; /**< resolutions array (constant) */
    uint32_t unused_32; /**< unused or temporary information */
    uint64_t unused_64; /**< unused or temporary information */
};

/**
 * @brief Store an image metadata.
 */
struct pict_metadata {
    char pict_id[MAX_PIC_ID + 1]; /**< image unique identifier */
    unsigned char SHA[SHA256_DIGEST_LENGTH]; /**< image hashcode */
    uint32_t res_orig[2]; /**< original image resolution */
    uint32_t size[NB_RES]; /**< byte sizes of different resolutions */
    uint64_t offset[NB_RES]; /**< image positions of different resolutions */
    uint16_t is_valid; /**< whether is image is used (NON_EMPTY) or not (EMPTY) */
    uint16_t unused_16; /**< unused or temporary information */
};

/**
 * @brief Store a database with its header and images.
 */
struct pictdb_file {
    FILE* fpdb; /**< disk file */
    struct pictdb_header header; /**< database header */
    struct pict_metadata metadata[MAX_MAX_FILES]; /**< images metadata */
};

/**
 * @brief Prints database header information.
 *
 * @param header The header to be displayed.
 */
void print_header (const struct pictdb_header* header);

/**
 * @brief Prints picture metadata information.
 *
 * @param metadata The metadata of one picture.
 */
void print_metadata (const struct pict_metadata* metadata);

/**
 * @brief Displays (on stdout) pictDB metadata.
 *
 * @param db_file In memory structure with header and metadata.
 */
void do_list (const struct pictdb_file* file);

/**
 * @brief Creates the database called db_filename. Writes the header and the
 *        preallocated empty metadata array to database file.
 *
 * @param db_file In memory structure with header and metadata.
 */
int do_create (const char* filename, struct pictdb_file* db_file);

/**
 * @brief Opens given file, reads header and metadata.
 *
 * @param filename Name of file to be opened.
 * @param mode File mode to be used (e.g. "rb", "wb").
 * @param db_file In memory structure with header and metadata.
 */
int do_open (const char* filename, const char* mode, struct pictdb_file* db_file);

/**
 * @brief Closes file included in db_file.
 *
 * @param db_file In memory structure with header and metadata.
 */
void do_close (struct pictdb_file* db_file);

/**
 * @brief Deletes an image.
 *
 * @param filename Name of file to be deleted.
 * @param db_file In memory structure with header and metadata.
 */
int do_delete (const char* filename, struct pictdb_file* db_file);

/* **********************************************************************
 * TODO WEEK 09: ADD THE PROTOTYPE OF resolution_atoi HERE.
 * **********************************************************************
 */

/* **********************************************************************
 * TODO WEEK 09: ADD THE PROTOTYPE OF do_read HERE.
 * **********************************************************************
 */

/* **********************************************************************
 * TODO WEEK 09: ADD THE PROTOTYPE OF do_insert HERE.
 * **********************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif
