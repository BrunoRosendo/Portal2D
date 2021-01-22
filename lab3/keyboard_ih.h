#ifndef KB_H
#define KB_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Wrapper function for the sys_inb() function. Increases the global variable counter
 * @return Returns 0 uppon success and non-zero otherwise
 */
int kbc_sys_inb(int port, uint8_t *value);

/**
 * @brief Function with the cycle of the interruption handling
 * @return Returns 0 uppon success and non-zero otherwise
 */
int kbc_int_cycle(uint8_t bit_no);

/**
 * @brief Function with the polling cycle
 * @return Returns 0 uppon success and non-zero otherwise
 */
int kbc_poll_cycle();

// kbc_ih() is already defined in lab3.h

/**
 * @brief Function which handles the keyboard and timer interruptions
 * @return Returns 0 uppon success and non-zero otherwise
 */ 
int kbd_timed_scan(uint8_t n, uint8_t bit_no_kbd, uint8_t bit_no_timer);

#endif
