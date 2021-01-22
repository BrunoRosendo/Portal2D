#ifndef UART_H
#define UART_H

#include <lcom/lcf.h>
#include "protocol.h"

/** @defgroup uart uart
 * @{
 *
 * Functions for using the UART's serial port
 */


#define COM1_IRQ 4
#define COM2_IRQ 3

#define COM1_BASE_ADDR 0x3F8 // until 0x3FF
#define COM2_BASE_ADDR 0x2F8 // until 0x2FF

// Register Offsets
#define REG_RBR 0
#define REG_THR 0
#define REG_IER 1
#define REG_IIR 2
#define REG_FCR 2
#define REG_LCR 3
#define REG_MCR 4
#define REG_LSR 5
#define REG_MSR 6
#define REG_SR 7

#define REG_DLL 0 // if bit DLAB of LCR is 1
#define REG_DLM 1

// LCR
#define LCR_BPC BIT(0) | BIT(1)
#define LCR_1SP 0
#define LCR_2SP BIT(2)
#define LCR_NO_PAR 0
#define LCR_ODD_PAR BIT(3)
#define LCR_EVEN_PAR BIT(3) | BIT(4)
#define LCR_PAR_IS_1 BIT(3) | BIT(5)
#define LCR_PAR_IS_0 BIT(3) | BIT(4) | BIT(5)
#define LCR_DLAB BIT(7)

// LSR
#define LSR_REC_DATA BIT(0)
#define LSR_OVER_ERR BIT(1)
#define LSR_PAR_ERR BIT(2)
#define LSR_FRAME_ERR BIT(3)
#define LSR_BREAK_INT BIT(4)
#define LSR_THRE BIT(5) // holding register empty
#define LSR_TER BIT(6) // bot THRE and shift register empty
#define LSR_FIFO_ERR BIT(7)

// IER
#define IER_DATA BIT(0)
#define IER_THRE BIT(1)
#define IER_RLS BIT(2) // receiver line status
#define IER_MODEM BIT(3)

// IIR
#define IIR_NO_INT BIT(0)
#define IIR_INT_RLS BIT(1) | BIT(2)
#define IIR_INT_DATA BIT(2)
#define IIR_INT_FIFO_TOUT BIT(2) | BIT(3) //character timeout
#define IIR_INT_THRE BIT(1)
#define IIR_INT_MODEM 0

// FCR
#define FCR_ENABE_FIFO BIT(0)
#define FCR_CLEAR_RCV BIT(1)
#define FCR_CLEAR_XMIT BIT(2)
#define FCR_8TRIGGER BIT(7) //sets the 

#define BIT_RATE_DIV 115200

static uint8_t uart_message[10];
static int bytes_count = 0;

// ALL FUNCTIONS USE COM1

/**
 * @brief Subscribes the interruptions of the UART
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_subscribe_int(uint8_t* bit_no, int* hook_id);

/**
 * @brief Unsubscribes the interruptions of the UART
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_unsubscribe_int(int* hook_id);

/**
 * @brief Reads a UART reg (COM1), specified by the reg_offset parameter (use the REG_XXX macros)
 * @param reg_offset Offset specifying the registed intended to read
 * @param data Variable which will store the register's information
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_read_reg(int reg_offset, uint8_t* data);

/**
 * @brief Writes to a UART reg (COM1), specified by the reg_offset parameter (use the REG_XXX macros)
 * @param reg_offset Offset specifying the registed intended to write to
 * @param data Variable storing the commmand/information intended to write to the register
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_write_reg(int reg_offset, uint8_t data);

/**
 * @brief Enables the interrupts desired for the project
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_enable_interrupts();

/**
 * @brief Disables all the interrupts
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_disable_interrupts();

/**
 * @brief Enables the DLL and DLM registers
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_enable_dlab();

/**
 * @brief Disables the DLL and DLM registers, going back to the default
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_disable_dlab();

/**
 * @brief Configures the uart
 * @param bpc Number of bytes per character
 * @param stop_bits Number of stop bits
 * @param par Type of parity error checking
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_config(uint8_t bpc, uint8_t stop_bits, uint8_t par);

/**
 * @brief Sets the UART's bitrate
 * @param bitrate Rate to change to
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_set_bitrate(unsigned int bitrate);


/**
 * @brief Starts the uart by subscribing its interrupts and configuring it for the project
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_init(uint8_t* bit_no, int* hook_id);

/**
 * @brief Unsubscribes UART's interrupts and disables everything related
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_end(int* hook_id);

/**
 * @brief Writes a character to the Transmitter Holding Register
 * @param byte Character intended to send
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_write_char(uint8_t byte);

/**
 * @brief Reads the available data in the RBR and checks if the message is over
 * @param data Changed to the array message if the message is over
 * @return True if the message is over and false otherwise
 */
bool uart_read_data(uint8_t data[]);

/**
 * @brief Reads and acknowledge byte
 * @param byte Variable to store the Ack into
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_read_ack(uint8_t* byte);

/**
 * @brief Writes a full message to the Transmitter Holding Register and checks the ACK bytes
 * @param message Array of chars to send
 * @param size Number of chars to send
 * @return Returns 0 upong success and non-zero otherwise
 */
int uart_write_message(uint8_t message[], int size);

/**
 * @brief Handles a UART interruptions
 * @param data Array which will contain a full message, when completed
 * @return True if received a complete message and false otherwise
 */
bool uart_int_handler(uint8_t data[]);

/**
 * @brief Stashes everything in the Receiver Buffer
 */
void stash_RBR();

#endif
