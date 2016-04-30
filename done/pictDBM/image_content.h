/**
 * @file image_content.h
 * @implementation of lazy_image to resize an image
 *
 * @author Aurélien Soccard & Teo Stocco
 * @date 2 April 2016
 */

#ifndef IMAGE_CONTENT_H
#define IMAGE_CONTENT_H

#include <stdio.h>
#include <stdint.h>

/**
 * @brief Computes the aspect ratio
 *
 * @param current_width The current width.
 * @param current_height The current height.
 * @param max_goal_width The maximum goal width.
 * @param max_goal_height The maximum goal height.
 */
double resize_ratio(int current_width, int current_height, int max_goal_width, int max_goal_height);

/**
 * @brief Computes the resize factor by keeping aspect ratio
 *
 * @param res The resize resolution.
 * @param db_file In memory structure with header and metadata.
 * @param index The picture db index.
 */
int lazy_resize(unsigned int res, struct pictdb_file *db_file, size_t index);

/**
 * @brief Gets the resolution of a JPEG image
 *
 * @param height image_buffer height
 * @param width image_buffer width
 * @param image_buffer image for getting height and width
 * @param image_size size of image buffer
 */
int get_resolution(uint32_t* height, uint32_t* width, const char* image_buffer, size_t image_size);

#endif
