#ifndef KBC_H
#define KBC_H

#include <stdbool.h>
#include <stdint.h>

#define KBC_IRQ 1

#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_ARG_REG 0x60
#define KBC_OUT_BUF 0x60

#define KBC_OBF BIT(0)
#define KBC_IBF BIT(1)
#define KBC_TO_ERR BIT(6)
#define KBC_PAR_ERR BIT(7)

#define ESC_KEY 0x81
#define SBYTE_CODE 0xE0

#define DELAY_US 20000

/**
 * @brief Subscribes the KBC interrupts
 * @return Returns 0 upong success and non-zero otherwise
 */
int subscribe_kbc(uint8_t* bit_no, int *hook_id);

/**
 * @brief Unsubscribes the KBC interrupts
 * @return Returns 0 uppon success and non-zero otherwise
 */
int unsubscribe_kbc(int *hook_id);

/**
 * @brief Enables the KBD interrupts
 * @return Returns 0 uppon succes and non-zero otherwise
 */
int enable_interrupts();

#endif
