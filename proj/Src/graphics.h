#ifndef GRAPH_H
#define GRAPH_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

/** @defgroup graphics graphics
 * @{
 *
 * Functions for using VBE
 */

#define VBE_LINEAR 1<<14

#define VBE_BIOS_CALL 0x10
#define SET_VBE_MODE 0x4F02
#define VBE_MODE_INFO 0x4F01
#define VBE_PAGE_FLIPPING 0x4F07
#define VBE_SET_VTRACE 0x80

#define VBE_ERR_CALL 0x01
#define VBE_ERR_HW 0x02
#define VBE_ERR_MODE 0X03
#define VBE_FINE 0

#define INDEXED_MODE 0X04
#define DIRECT_MODE 0X06

#define SET_INDEXED_MODE 0X105
#define SET_XPM_5_6_5 0X11A


/**
 * @brief Gets a mode's information and puts it in a vbe_mode_info_t struct
 * @param mode Mode to get the information from
 * @param vmi_p Struct which will contain the information
 * @return 0 on success, non-zero otherwise
 */
int vbe_get_mode_info2(uint16_t mode, vbe_mode_info_t *vmi_p);

/**
 * @brief Draws a pixel with a given color at a given position
 * @param x X position of the pixel
 * @param y Y position of the pixel
 * @param color Color of the pixel
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief Gets a pixel and stores it
 * @param x X position of the pixel
 * @param y Y position of the pixel
 * @param color Variable where the color will be stored
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_get_pixel(uint16_t x, uint16_t y, uint16_t* color);

/**
 * @brief Draws a pixel from the background buffer
 * @param x X position
 * @param y Y position
 */
int vg_draw_from_background(uint16_t x, uint16_t y);

/**
 * @brief Parses the color and puts the 3 colors in the respective variables (in LSB)
 * @param color Full color
 * @param red Variable which will contain the red part of the color
 * @param green Variable which will contain the red part of the green
 * @param blue Variable which will contain the red part of the blue
 */
void get_rgb(uint32_t color, uint32_t* red, uint32_t* green, uint32_t* blue);

/**
 * @brief Copies the pixels in aux_buffer to the main buffer
 */
void buffer_screen();

/**
 * @brief Gets the type of the xpm
 * @return Enumerable containing the type of the xpm
 */
enum xpm_image_type get_type();

/**
 * @brief Gets the horizontal resolution of the game
 * @return Horizontal resolution of the game
 */
unsigned get_h_res();

/**
 * @brief Gets the vertical resolution of the game
 * @return Vertical resolution of the game
 */
unsigned get_v_res();

/**
 * @brief Changes the state of the main_buffer variable to !main_buffer
 */
void flip_page();

/**
 * @brief Calls vg_exit() and frees the buffers
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_exit_program();

/**
 * @brief Copies the content in the video buffer to the background buffer
 */
void load_background();

/**
 * @brief Paints all the pixels black
 */
void erase_everything();

#endif
