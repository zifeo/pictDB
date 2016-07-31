// Provided file © EPFL

#include <stdint.h> // for uint16_t, uint32_t
#include <errno.h>
#include <inttypes.h> // strtoumax()

uint16_t atouint16(const char* str) {
    char* endptr;
    errno = 0;
    uintmax_t val = strtoumax(str, &endptr, 10);
    if (errno == ERANGE || val > UINT16_MAX || endptr == str || *endptr != '\0') {
        errno = ERANGE;
        return 0;
    }
    return (uint16_t) val;
}

uint32_t atouint32(const char* str) {
    char* endptr;
    errno = 0;
    uintmax_t val = strtoumax(str, &endptr, 10);
    if (errno == ERANGE || val > UINT32_MAX || endptr == str || *endptr != '\0') {
        errno = ERANGE;
        return 0;
    }
    return (uint32_t) val;
}
