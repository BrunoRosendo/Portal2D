#ifndef PS2_H
#define PS2_H

#include <stdbool.h>
#include <stdint.h>
#include "kbc.h"

/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the PS2's mouse
 */


#define PS2_IRQ 12

#define PS2_LB BIT(0)
#define PS2_RB BIT(1)
#define PS2_MB BIT(2)
#define PS2_XDELTA_MSB BIT(4)
#define PS2_YDELTA_MSB BIT(5)
#define PS2_XOV BIT(6)
#define PS2_YOV BIT(7)

#define PS2_WRITE_BYTE 0xD4
#define PS2_DIS_DATA 0xF5
#define PS2_EN_DATA 0xF4
#define PS2_SET_STREAMMODE 0XEA
#define PS2_SET_REMMODE 0XF0
#define PS2_READ_DATA 0XEB

#define PS2_ACK_OK 0xFA
#define PS2_NACK 0XFE
#define PS2_ERROR 0XFC

static uint8_t bytes[3];
static int num_bytes = 0;

/**
 * @brief Enumerated type for specifying a mouse event
 */
typedef enum mouse_event{
    MOUSE_LB_PRESSED,
    MOUSE_RB_PRESSED,
    MOUSE_MOVEMENT,
    MOUSE_OTHER_EV
} mouse_event;

/**
 * @brief Enables data reporting 
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_enable_data_reporting2();

/**
 * @brief Subscribes the mouse interrupts
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no, int* hook_id);

/**
 * @brief Writes a command to the mouse driver
 * @param cmd Command
 * @return Returns 0 upong success and non-zero otherwise
 */
int write_mouse_cmd(uint8_t cmd);

/**
 * @brief Disables data reporting
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_disable_data_reporting();

/**
 * @brief Unsubscribes mouse interruptions
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_unsubscribe_int(int *hook_id);

/**
 * @brief Restores the KBC and mouse to the Minix's default
 * @return Returns 0 uppon success and non-zero otherwise
 * */
int restore_kbc_and_mouse();

/**
 * @brief Constructs a struct packet based on an array of 3 bytes
 * @param pp Pointer to the struct which will be constructed
 * @param bytes Array containing a mouse packet
 */
void construct_pp(struct packet *pp, uint8_t bytes[]);

/**
 * @brief Handles mouse interruptions and saves the information in 3 variables when the packet is completed
 * @param event Variable used to store the mouse event
 * @param x_mov Variable used to store the moviment in the x axis (0 by default)
 * @param y_mov Variable used to store the moviment in the y axis (0 by default)
 * @return True if packet is completed and false otherwise
 */
bool mouse_int_handler(mouse_event* event, int16_t* x_mov, int16_t* y_mov);

#endif
