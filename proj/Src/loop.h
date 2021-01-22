#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <lcom/lcf.h>
#include "game.h"
#include "rtc.h"
#include "uart.h"

/** @defgroup loop loop
 * @{
 *
 * Functions for the main loop of the game, aswell as the start and end of the program
 */


static uint8_t bit_no_timer, bit_no_kbc, bit_no_mouse, bit_no_rtc, bit_no_uart;
static int hook_id_kbc, hook_id_mouse, hook_id_rtc, hook_id_uart;

/**
 * @brief Starts the game. Subscribes all the interrupts
 * @return Returns 0 uppon success and non-zero otherwise
 */
int start_loop(char resources_path[]);

/**
 * @brief Main loop of the game. Handles all the interruptions
 * @return Returns 0 uppon success and non-zero otherwise
 */
int main_loop();

/**
 * @brief Ends the game and exits the program. Unsubscribes interrupts
 * @return Returns 0 uppon success and non-zero otherwise
 */
int end_loop();

#endif
