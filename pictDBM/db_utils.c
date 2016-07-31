
#include "pictDB.h"

#include <inttypes.h>
#include <string.h>

/********************************************************************//**
 * Human-readable SHA
 */
static void sha_to_string (const unsigned char* SHA, char* sha_string)
{
    if (SHA == NULL) {
        return;
    }
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(&sha_string[i*2], "%02x", SHA[i]);
    }
    sha_string[2*SHA256_DIGEST_LENGTH] = '\0';
}

/********************************************************************//**
 * PictDB header display.
 */
void print_header (const struct pictdb_header* header)
{
    if (header == NULL) {
        return;
    }

    puts("*****************************************");
    puts("**********DATABASE HEADER START**********");
    printf("DB NAME:%31s\n", header->db_name);
    printf("VERSION: %" PRIu32 "\n", header->db_version);
    printf("IMAGE COUNT: %" PRIu32 "\t\tMAX IMAGES: %" PRIu32 "\n", header->num_files, header->max_files);
    printf("THUMBNAIL: %" PRIu16 " x %" PRIu16 "\tSMALL: %" PRIu16 " x %" PRIu16 "\n",
           header->res_resized[RES_THUMB], header->res_resized[RES_THUMB + 1], header->res_resized[2 * RES_SMALL],
           header->res_resized[2 * RES_SMALL + 1]);
    puts("***********DATABASE HEADER END***********");
    puts("*****************************************");
}

/********************************************************************//**
  * Metadata display.
  */
void print_metadata(const struct pict_metadata* metadata)
{
    if (metadata == NULL) {
        return;
    }

    char sha_printable[2 * SHA256_DIGEST_LENGTH + 1];
    sha_to_string(metadata->SHA, sha_printable);

    printf("PICTURE ID: %s\n", metadata->pict_id);
    printf("SHA: %s\n", sha_printable);
    printf("VALID: %" PRIu16 "\n", metadata->is_valid);
    printf("UNUSED: %" PRIu16 "\n", metadata->unused_16);
    printf("OFFSET ORIG. : %" PRIu64 "\t\tSIZE ORIG. : %" PRIu32 "\n",
           metadata->offset[RES_ORIG], metadata->size[RES_ORIG]);
    printf("OFFSET THUMB.: %" PRIu64 "\t\tSIZE THUMB.: %" PRIu32 "\n",
           metadata->offset[RES_THUMB], metadata->size[RES_THUMB]);
    printf("OFFSET SMALL : %" PRIu64 "\t\tSIZE SMALL : %" PRIu32 "\n",
           metadata->offset[RES_SMALL], metadata->size[RES_SMALL]);
    printf("ORIGINAL: %" PRIu32 " x %" PRIu32 "\n", metadata->res_orig[0], metadata->res_orig[1]);
    puts("*****************************************");
}

/********************************************************************//**
 * Opens given file, reads header and metadata.
 */
int do_open (const char* filename, const char* mode, struct pictdb_file* db_file)
{
    M_REQUIRE_NON_NULL(filename);
    M_REQUIRE_NON_NULL(mode);
    M_REQUIRE_NON_NULL(db_file);

    M_REQUIRE_VALID_FILENAME(filename);

    db_file->fpdb = fopen(filename, mode);
    db_file->metadata = NULL;

    if (db_file->fpdb == NULL) {
        return ERR_IO;
    }

    int status = 0;
    if (fread(&db_file->header, sizeof(struct pictdb_header), 1, db_file->fpdb) != 1) {
        status = ERR_IO;
    } else {
        db_file->metadata = (struct pict_metadata *) calloc(db_file->header.max_files, sizeof(struct pict_metadata));

        if (db_file->metadata == NULL) {
            status = ERR_OUT_OF_MEMORY;
        } else if (fread(db_file->metadata, sizeof(struct pict_metadata), db_file->header.max_files, db_file->fpdb) !=
                   db_file->header.max_files) {
            status = ERR_IO;
        } else if (db_file->header.max_files > MAX_MAX_FILES) {
            status = ERR_MAX_FILES;
        }
    }

    if (status != 0) {
        do_close(db_file);
    }

    return 0;
}

/********************************************************************//**
 * Closes file included in db_file.
 */
void do_close (struct pictdb_file* db_file)
{
    if (db_file == NULL) {
        return;
    }

    if (db_file->metadata != NULL) {
        free(db_file->metadata);
        db_file->metadata = NULL;
    }

    if (db_file->fpdb != NULL) {
        fclose(db_file->fpdb);
    }

}

/********************************************************************//**
 * Closes file included in db_file.
 */
int resolution_atoi(const char *resolution)
{
    if (resolution == NULL) {
        return -1;
    }
    if (!strncmp(NAME_RES_THUMB, resolution, NAME_RES_MAX) || !strncmp(NAME_RES_THUMBNAIL, resolution, NAME_RES_MAX)) {
        return RES_THUMB;
    }
    if (!strncmp(NAME_RES_SMALL, resolution, NAME_RES_MAX)) {
        return RES_SMALL;
    }
    if (!strncmp(NAME_RES_ORIG, resolution, NAME_RES_MAX) || !strncmp(NAME_RES_ORIGINAL, resolution, NAME_RES_MAX)) {
        return RES_ORIG;
    }
    return -1;
}

