// Provided file © EPFL

#ifndef PICTDBPRJ_PICTDBM_TOOLS_H
#define PICTDBPRJ_PICTDBM_TOOLS_H

#include <stdint.h> // for uint16_t, uint32_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String to uint16_t conversion function
 *
 * @param str a string containing some integer value to be extracted
 * @return converted value in uint16_t format
 */
uint16_t atouint16(const char* str);

/**
 * @brief String to uint32_t conversion function
 *
 * @param str a string containing some integer value to be extracted
 * @return converted value in uint32_t format
 */
uint32_t atouint32(const char* str);

#ifdef __cplusplus
}
#endif
#endif
