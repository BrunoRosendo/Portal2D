#ifndef PS2
#define PS2

#include <stdbool.h>
#include <stdint.h>

// File with reusable functions

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

/**
 * @brief Enables data reporting 
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_enable_data_reporting2();

/**
 * @brief Subscribes the mouse interrupts
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no, int* hook_id);

/**
 * @brief Reads a byte from the KFC_OUT_BUF
 * @return Returns 0 upong success and non-zero otherwise
 */ 
int read_out_buffer(uint8_t* byte);

/**
 * @brief Writes a command to the mouse driver
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
 * @return Returns 0 upong success and non-zero otherwise
 */
int mouse_unsubscribe_int(int *hook_id);

/**
 * @brief Checks if we can write to the Input Buffer
 * @return True if able and false otherwise
 */
bool kbc_able_to_write();

/**
 * @brief Checks if we can read from the Output Buffer
 * @return True if able and false otherwise
 */
bool kbc_able_to_read();

/**
 * @brief Writes a command to the kbc driver
 * @return Returns 0 upong success and non-zero otherwise
 */
int kbc_write_cmd(uint8_t cmd);

/**
 * @brief Writes an argument to the KBC_ARG Register
 * @return Returns 0 upong success and non-zero otherwise
 */
int kbc_write_arg(uint8_t arg);

/**
 * @brief Restores the KBC and mouse to the Minix's default
 * @return Returns 0 uppon success and non-zero otherwise
 * */
int restore_kbc_and_mouse();

#endif
