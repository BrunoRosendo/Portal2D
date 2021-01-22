#ifndef GRAPH_H
#define GRAPH_H
#include <stdint.h>
#include <stdio.h>

#define VBE_LINEAR 1<<14

#define VBE_BIOS_CALL 0x10
#define SET_VBE_MODE 0x4F02
#define VBE_MODE_INFO 0x4F01

#define VBE_ERR_CALL 0x01
#define VBE_ERR_HW 0x02
#define VBE_ERR_MODE 0X03
#define VBE_FINE 0

#define INDEXED_MODE 0X04
#define DIRECT_MODE 0X06

#define SET_INDEXED_MODE 0X105

/**
 * @brief same as vbe_get_mode_info
 * @return 0 on success, non-zero otherwise
 */
int vbe_get_mode_info2(uint16_t mode, vbe_mode_info_t *vmi_p);

/**
 * @brief Draws a pixel with a given color at a given position
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

/**
 * @brief Draws a given number of rectangles
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_draw_pattern(uint8_t no_rectangles, uint32_t first, uint8_t step);

/**
 * @brief Parses the color and puts the 3 colors in the respective variables (in LSB)
 */
void get_rgb(uint32_t color, uint32_t* red, uint32_t* green, uint32_t* blue);

/**
 * @brief Draws a xpm file at a given position
 * @return Returns 0 uppon success and non-zero otherwise
 */
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y, enum xpm_image_type type);

/**
 * @brief Animates a sprite in a given xpm
 * @return Returns 0 uppon success and non-zero otherwise
 */
int animate_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate, enum xpm_image_type type);


int draw_sprite(uint8_t* sprite, xpm_image_t* img, uint16_t x, uint16_t y);

int destroy_sprite(uint8_t* sprite, xpm_image_t* img, uint16_t x, uint16_t y);

#endif
