#include "error.h"

const char * const ERROR_MESSAGES[] = {
        "", // no error
        "I/O Error",
        "(re|m|c)alloc failed",
        "Not enough arguments",
        "Invalid filename",
        "Invalid command",
        "Invalid argument",
        "Invalid max_files number",
        "Invalid resolution(s)",
        "Invalid picture ID",
        "Full database",
        "File not found",
        "Not implemented",
        "Existing picture ID",
        "Vips error",
        "Debug"
};

const size_t ERROR_COUNT = sizeof(ERROR_MESSAGES) / sizeof(ERROR_MESSAGES[0]);
