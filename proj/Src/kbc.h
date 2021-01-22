#ifndef KBC_H
#define KBC_H

#include <stdbool.h>
#include <stdint.h>

/** @defgroup kbc kbc
 * @{
 *
 * Functions for using the kbc driver
 */


#define KBC_IRQ 1

#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_ARG_REG 0x60
#define KBC_OUT_BUF 0x60

#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define KBC_TO_ERR BIT(6)
#define KBC_PAR_ERR BIT(7)

// Scan codes
#define KBC_INV_KEY 0
#define ESC_KEY_PRESSED 0x01 
#define ESC_KEY_RELEASED 0x81
#define A_KEY_PRESSED 0X1E
#define A_KEY_RELEASED 0X9E
#define D_KEY_PRESSED 0x20
#define D_KEY_RELEASED 0xA0
#define W_KEY_PRESSED 0x11
#define SPACE_KEY_PRESSED 0x39
#define S_KEY_PRESSED 0x1F
#define E_KEY_PRESSED 0x12

#define SBYTE_CODE 0xE0 // keys with 2 bytes

#define KBC_WRITE_CMDBYTE 0X60

#define DELAY_US 20000

/**
 * @brief Subscribes the KBC interrupts
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int kbc_subscribe_int(uint8_t* bit_no, int *hook_id);

/**
 * @brief Unsubscribes the KBC interrupts
 * @param hook_id ID of the subscription
 * @return Returns 0 uppon success and non-zero otherwise
 */
int kbc_unsubscribe_int(int *hook_id);

/**
 * @brief Reads a byte from the KFC_OUT_BUF
 * @param byte Variable to store the byte read
 * @return Returns 0 upong success and non-zero otherwise
 */ 
int read_out_buffer(uint8_t* byte);

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
 * @param cmd Command
 * @return Returns 0 upong success and non-zero otherwise
 */
int kbc_write_cmd(uint8_t cmd);

/**
 * @brief Writes an argument to the KBC_ARG Register
 * @param arg Argument
 * @return Returns 0 upong success and non-zero otherwise
 */
int kbc_write_arg(uint8_t arg);

/**
 * @brief Handles an interruptions and reads the information to byte
 * @param byte Variable intended to write the information to. NULL if there was an error
 */
void kbc_int_handler(uint8_t* byte);

#endif
