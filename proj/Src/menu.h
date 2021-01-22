#ifndef MENU_H
#define MENU_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "sprites.h"
#include "graphics.h"

/** @defgroup menu menu
 * @{
 *
 * Functions for the menu of the game
 */


Sprite* background;
Sprite* exit_but, *play_but, *editor_but, *coop_but;

/**
 * @brief Creates all the menu's sprites
 * @return Returns 0 uppon success and non-zero otherwise
 */
int start_menu();

/**
 * @brief Exits the menu and destroys all its sprites
 * @return Returns 0 uppon success and non-zero otherwise
 */
int exit_menu();

/**
 * @brief Gets the button in which the user has the mouse on (0 if none)
 * @param x Position of the mouse in the X axis
 * @param y Position of the mouse in the Y axis
 * @return Returns a character representing the button ('p': play, 'c': co-op, 'e': exit, 'l': editor)
 */
char check_buttons(int x, int y);

/**
 * @brief Draws the menu
 */
void draw_menu();

/**
 * @brief Erases the menu's sprites
 */
void erase_menu();

#endif
