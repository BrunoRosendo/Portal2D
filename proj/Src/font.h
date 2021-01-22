#ifndef FONT_H
#define FONT_H

#include <lcom/lcf.h>
#include "sprites.h"
#include "xpm_files.h"
#include "rtc.h"

/** @defgroup font font
 * @{
 *
 * Functions for using a font representing date and time
 */

#define GET_SECOND 0x0F
#define GET_FIRST 0xF0

static Sprite* numbers[10];
static Sprite* week_days[7][3];
static Sprite* slash_date, *two_dots;


/**
 * @brief Loads the font's sprites
 */
void start_font();

/**
 * @brief Frees the font's sprites
 */
void end_font();

/**
 * @brief Parses a number in BCD and returns two sprites representing the 2 digits
 * @param num Number intended to represent
 * @param x X position of the number
 * @param y Y position of the number
 * @param first Sprite which will store the first digit
 * @param second Sprite which will store the second digit
 */
void get_pair_of_digits(uint8_t num, int x, int y, Sprite* first, Sprite* second);

/**
 * @brief Parses a week day (BCD) and returns an array of 3 sprites representing the day (i.e. Sat)
 * @param day Day intended to represent (1 == Monday and 7 == Sunday)
 * @param x X position of the day
 * @param y Y position of the day
 * @param week_day Array of sprites which will containg the digits
 */
void get_week_day_sprite(uint8_t day, int x, int y, Sprite* week_day[]);

/**
 * @brief Gets the dots Sprite at a given position
 * @param x X position of the dots
 * @param y Y position of the dots
 */
Sprite* get_dots(int x, int y);

/**
 * @brief Gets the slash Sprite at a given position
 * @param x X position of the slash
 * @param y Y position of the slash
 */
Sprite* get_slash(int x, int y);

/**
 * @brief Draws all the time and dates sprites and saves them to a given array
 * @param old_sprites Array where the sprites are saved to be erased later
 */
void draw_time(Sprite old_sprites[]);

#endif
