#ifndef MOUSE_h
#define MOUSE_h

#include <stdbool.h>
#include <stdint.h>

// File with lab specific functions

/**
 * @brief Mouse interruption handling cycle
 * @return Returns 0 uppon success and non-zero otherwise
 */ 
int mouse_int_cycle(uint8_t bit_no, uint32_t cnt);

/**
 * @brief Constructs the struck packet pp used in mouse_print_packet
 */
void construct_pp(struct packet *pp, uint8_t bytes[]);

/**
 * @brief Mouse interruption cycle for mouse_test_async
 * @return Returns 0 uppon success and non-zero otherwise
 */
int mouse_timer_int_cycle(uint8_t bit_no, uint8_t bit_no_t0, uint8_t idle_time);

/**
 * @brief Mouse polling cycle in remote mode
 * @return Returns 0 uppon success and non-zero otherwise
 */
int mouse_remote_cycle(uint16_t period, uint8_t cnt);

int mouse_gesture_cycle(uint8_t bit_no, uint8_t x_len, uint8_t tolerance);


#endif
