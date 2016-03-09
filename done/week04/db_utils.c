/* ** NOTE: undocumented in Doxygen
 * @file db_utils.c
 * @brief implementation of several tool functions for pictDB
 *
 * @author Mia Primorac
 * @date 2 Nov 2015
 */

#include "pictDB.h"

#include <stdint.h> // for uint8_t
#include <inttypes.h>
#include <stdio.h> // for sprintf
#include <openssl/sha.h> // for SHA256_DIGEST_LENGTH

/********************************************************************//**
 * Human-readable SHA
 */
static void sha_to_string (const unsigned char* SHA,
               char* sha_string)
{
    if (SHA == NULL) {
        return;
    }
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(&sha_string[i*2], "%02x", SHA[i]);
    }

    sha_string[2*SHA256_DIGEST_LENGTH] = '\0';
}

void printNtimes(const char c, unsigned int n){
    for (int i=0;i<n;++i)
        printf("%c", c);
}


/********************************************************************//**
 * pictDB header display.
 */
void print_header (const struct pictdb_header header){
    printNtimes('*', 41);
    printf("\n");
    printNtimes('*', 10);
    printf("DATABASE HEADER START");
    printNtimes('*', 10);
    printf("\n");
    printf("DB NAME:%31s\n", header.db_name);
    printf("DB VERSION: %" PRIu32 "\n", header.db_version);
    printf("IMAGE COUNT: %" PRIu32 "\t\tMAX IMAGES: %" PRIu32 "\n", header.num_files, header.max_files);
    printf("THUMBAIL: %" PRIu16 " x %" PRIu16 "\tSMALL: %" PRIu16 " x %" PRIu16"\n", header.res_resized[0],header.res_resized[1], header.res_resized[2],header.res_resized[3]);
    printf("\n");
    printNtimes('*', 11);
    printf("DATABASE HEADER END");
    printNtimes('*', 11);
    printf("\n");
}

/********************************************************************//**
 * Metadata display.
 */
void print_metadata (const struct pict_metadata metadata)
{
    char sha_printable[2*SHA256_DIGEST_LENGTH+1];
    sha_to_string(metadata.SHA, sha_printable);

    /* **********************************************************************
     * TODO: WRITE YOUR CODE HERE
     * **********************************************************************
     */
}
