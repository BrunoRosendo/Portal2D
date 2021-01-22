#ifndef SPRITE_H
#define SPRITE_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "graphics.h"

/** @defgroup sprites sprites
 * @{
 *
 * Functions for using sprites
 */


/**
 * @brief Struct defining a Sprite
 */
typedef struct {
    int x, y; // current position
    int width, height; // dimensions
    int xspeed, yspeed; // current speed
    uint8_t *map; // the pixmap
    uint8_t *map_reversed; // Load it and use flip_sprite() if you want to use it
} Sprite;

/**
 * @brief Enumerated type for specifying the type of a sprite (useful for the level editor)
 */
typedef enum Sprite_type{
    WHITE_BLOCK,
    BLACK_BLOCK,
    CHARACTER,
    FLAG,
    DELETE
} Sprite_type;

/**
 * @brief Creates a new struct Sprite
 * @param pic xpm struct representing the sprite
 * @param x X position of the sprite
 * @param y Y position of the sprite
 * @param xpseed X speed of the sprite
 * @param yspeed Y speed of the sprite
 */
Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed);

/**
 * @brief Destroys a given Sprite
 * @param sp Sprite intended to destroy
 */
void destroy_sprite(Sprite *sp);

/**
 * @brief Draws a given Sprite
 * @param sp Sprite intended to draw
 */
void draw_sprite(Sprite* sp);

/**
 * @brief Erases the sprite by painting it all in black
 * @param sp Sprites intended to erase
 */
void erase_sprite(Sprite* sp);

/**
 * @brief Animates a given Sprite based on its xspeed and yspeed
 * @param sp Sprite you want to animate
 */
void animate_sprite(Sprite* sp);

/**
 * @brief Flips a sprite by changing its map to map_reversed
 * @param sp Sprite you want to flip
 * @return Returns 0 uppon success and non-zero otherwise
 */
int flip_sprite(Sprite* sp);

#endif
