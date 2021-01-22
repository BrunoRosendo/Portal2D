#ifndef ASPRITE_H
#define ASPRITE_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "sprites.h"
#include "xpm_files.h"

/** @defgroup asprite asprite
 * @{
 *
 * Functions for using Animated Sprites
 */

/**
 * @brief Enumerated type for specifying the type of the portal (an animated sprite)
 */
typedef enum portal_type{
    PURPLE_LEFT,
    PURPLE_RIGHT,
    PURPLE_UP,
    PURPLE_DOWN,
    GREEN_LEFT,
    GREEN_RIGHT,
    GREEN_UP,
    GREEN_DOWN
} portal_type;

/**
 * @brief Struct defining a portal, which includes an animated sprite
 */
typedef struct{
    Sprite* sp; // standard sprite
    int aspeed; // no. frames per pixmap
    int frames_left; // no. frames left to next change
    int num_fig; // number of pixmaps
    int cur_fig; // current pixmap
    uint8_t** map; // array of pointers to pixmaps
} ASprite;

/**
 * @brief Creates a portal asprite with given type, x and y
 * @param type Type of the portal
 * @param x X position of the protal
 * @param y Y position of the portal
 * @return An Asprite with the portal information or NULL in case of an error
 */
ASprite* create_portal(portal_type type, int x, int y);

/**
 * @brief Creates an animated sprite
 * @param num_pic Number of pictures in the ASprite
 * @param pics Array containing the pictures of the ASprite
 * @param aspeed Number of frames needed for the picture to change
 * @param x X Position of the ASprite
 * @param y Y Position of the ASprite
 * @param xseep X speed of the ASprite
 * @param yspeed Y speed of the ASprite
 * @return An Asprite* or NULL in case of an error
 */
ASprite* create_asprite(int num_pic, xpm_map_t pics[], int aspeed, int x, int y, int xspeed, int yspeed);

/**
 * @brief Destroys a given animated sprite
 * @param asp ASprite to destroy
 */
void destroy_asprite(ASprite* asp);

/**
 * Animates a given sprite. Draws the new sprite but doesn't erase the old one (must be done later because of page flipping)
 * @brief Animates a given sprite
 * @param asp ASprite to animate
 */
void animate_asprite(ASprite* asp);

#endif
