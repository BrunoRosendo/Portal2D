#ifndef GAME_H
#define GAME_H

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "sprites.h"
#include "graphics.h"
#include "xpm_files.h"
#include "kbc.h"
#include "asprite.h"
#include "mouse.h"
#include "menu.h"
#include "rtc.h"
#include "font.h"
#include "uart.h"

/** @defgroup game game
 * @{
 *
 * Functions, variables and constants for the main logic and drawing of the game
 */

// Defining character states
#define STILL_RIGHT 0
#define STILL_LEFT 1
#define MOVING_LEFT 2
#define MOVING_RIGHT 3
#define LEFT_AND_THEN_RIGHT 4
#define RIGHT_AND_THEN_LEFT 5

/**
 * @brief States for state machine managing the game
 */
typedef enum Game_state{ // to play coop, the players need to have the same levels!
    MENU,
    MAIN_GAME,
    LEVEL_EDITOR,
    COOP,
    OVER
} Game_state;

// Variables of the class game

static Game_state game_state = MENU;
static Sprite** blocks;
static Sprite* end_flag = NULL;
static ASprite* purple_portal = NULL;
static ASprite* green_portal = NULL;
static Sprite old_purple;
static Sprite old_green;
static portal_type green_type;
static portal_type purple_type;
static char grid[16][20];   // grid of a given level, 0 = empty, 1 = white block, 2 = black block
                            // 3 = character spawn (editor), 4 = end flag (editor)

static const unsigned int game_freq = 60;
static const unsigned int block_width = 64;
static int block_num;
static int frame_counter = 0;

static bool editor_update = false;
static int current_level = 0;
static char resources_path[50];

// PLAYER 1
static Sprite* character = NULL;
static Sprite* cursor;
static Sprite old_cursor;
static char char_state = 0;     // maybe create a struct Character
static bool char_reversed = false;
static uint8_t* reversed_img;

// PLAYER 2
static Sprite* character2 = NULL;
static char char_state2 = 0;     // maybe create a struct Character
static bool char_reversed2 = false;
static uint8_t* reversed_img2;

// time sprites
static Sprite old_time[19];


// Functions managing the game itself

/**
 * @brief Starts a stage of the game (menu, single/coop, editor)
 * @return Returns 0 uppon success and non-zero otherwise
 */
int game_init();

/**
 * @brief Ends a stage of the game (menu, single/coop, editor)
 * @return Returns 0 uppon success and non-zero otherwise
 */
int game_end();

/**
 * @brief Checks if the player has completed the level
 * @return True if it's the player completed the level and false otherwise
 */
bool check_level_end();

/**
 * @brief Checks the collisions with everyblock and updates the character accordingly
 * @param blockswp Array which will contain the indexes of the blocks containing a portal with a character inside
 * @param num Number of blocks containing a portal with a character inside
 * @param charc Character intended to check
 */
void check_block_collisions(int blockswp[], int* num, Sprite* charac);

/**
 * @brief Checks if the player is entering/leaving a portal (teleporting only)
 * @param character Character intended to check
 */
void check_portals(Sprite* character);

/**
 * @brief Teleports the character from a portal to another
 * @param purple True if the character arrived at the purple portal and false if it arrived at the green portal
 * @param character Character intended to check
 */
void teleport_character(bool purple, Sprite* character);

/**
 * @brief Checks if a given block has a portal in it
 * @param b Sprite containing the block
 */
bool block_has_portal(Sprite* b);

/**
 * @brief Checks if there's a block at a given positions and returns it
 * @param x X position of the block
 * @param y Y position of the block
 * @return Block's Sprite if found and NULL otherwise
 */
Sprite* find_block(int x, int y);

/**
 * @brief state machine based on key events, which manages the characters
 * @param data Data of the key event
 * @param character Sprite contatining the character intended to manage
 * @param char_state State of the character
 */
void key_char_event(uint8_t* data, Sprite* character, char* char_state);

/**
 * @brief Handles a key event
 * @param data Data of the key event
 */
void key_event_handler(uint8_t* data);

/**
 * @brief Handles a mouse event and updates the cursor accordingly
 * @param event Event we need to handle
 * @param x_mov X axis movement
 * @param y_mov Y axis movement
 */
void mouse_event_handler(mouse_event event, int16_t x_mov, int16_t y_mov);

/**
 * @brief Handles an event caused by receiving a message from the serial port
 * @param data Array containing the message (including a header expliciting the type of the message and a tail)
 */
void uart_event_handler(uint8_t data[]);

/**
 * @brief Shoots a portal and calculates where it will land based on the position of the player and a given position
 * @param purple True if a purple portal was shot and false if a green purple was shot
 * @param character Character intended to check
 * @param x_c X position of the reference point to create a projectile (it's usually the mouse)
 * @param y_c Y position of the reference point to create a projectile (it's usually the mouse)
 */
void shoot_portal(bool purple, Sprite* character, int x_c, int y_c);

/**
 * @brief Destroys all the sprites and frees the memory
 */
void destroy_all_sprites();

/**
 * @brief Reads the next level from its respective file
 * @return Returns 0 uppon success and non-zero otherwise
 */
int read_level_from_file();

/**
 * @brief Initiates the level editor
 * @return Returns 0 uppon success and non-zero otherwise
 */
int editor_init();

/**
 * @brief Closes the level editor and writes the level to a text file
 * @return Returns 0 uppon success and non-zero otherwise
 */
int editor_end();

/**
 * @brief Checks if the character is colliding with a portal
 * @return True if the character is in a portal and false otherwise
 */
bool in_a_portal(Sprite* charac);

/**
 * @brief Checks if the user clicked in a button and changes the game state if he did
 */
void menu_change();

/**
 * @brief Gets the state of the game
 * @return Enumerable containing the state of the game
 */
Game_state get_game_state();

/**
 * @brief Sets the path to the resources folder
 * @param resources_path Path to the folder. Must not be greater than 50 characters
 */
void set_resources_path(char path[]);


// Drawing Functions

/**
 * @brief Draws the level of the game and loads it to the background buffer
 */
void draw_level();

/**
 * @brief Draws a new frame and erases the old one
 */
void draw_new_frame();

/**
 * @brief Draws an element of the level
 * @param type Type of the sprite the user wants to draw
 * @return Returns 0 uppon success and non-zero otherwise
 */
int editor_draw_element(Sprite_type type);

#endif
