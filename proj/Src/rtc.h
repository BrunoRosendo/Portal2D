#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

/** @defgroup rtc rtc
 * @{
 *
 * Functions for using the Real-Time Clock
 */


#define RTC_ADDR_REG 0x70 // load it with the rtc register
#define RTC_DATA_REG 0x71 // data to the register

#define REG_A 0x0A
#define REG_B 0x0B
#define REG_C 0x0C
#define REG_D 0x0D

#define REG_SECS 0
#define REG_MINS 2
#define REG_HRS 4
#define REG_WEEK_DAY 6
#define REG_MONTH_DAY 7
#define REG_MONTH 8
#define REG_YEAR 9

#define RTC_IRQ 8

#define SET_UPDATES BIT(4)
#define UPDATE_PENDING BIT(4)

static uint8_t week_day, hours, minutes, seconds, year, month, day; // update them in int_handler and return them to update frame

/**
 * @brief Subscribes the RTC's interruptions
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @param hook_id ID of the subscription
 * @return Returns 0 upong success and non-zero otherwise
 */
int rtc_subscribe_int(uint8_t* bit_no, int *hook_id);

/**
 * @brief Unsubscribes the RTC's interrupts
 * @param hook_id ID of the subscription
 * @return Returns 0 uppon success and non-zero otherwise
 */
int rtc_unsubscribe_int(int *hook_id);

/**
 * @brief Reads a given RTC register
 * @param reg Register intended to read
 * @param byte Variable to store the register's information
 * @return Returns 0 upong success and non-zero otherwise
 */
int rtc_read_reg(uint8_t reg, uint8_t* byte);

/**
 * @brief Writes a command to a given RTC register
 * @param reg Register intended to write to
 * @param byte Command/information which will be written to the register
 * @return Returns 0 upong success and non-zero otherwise
 */
int rtc_write_reg(uint8_t reg, uint8_t byte);

/**
 * @brief Enables interruptions for time updates
 * @return Returns 0 upong success and non-zero otherwise
 */
int enable_int_time_updates();

/**
 * @brief Disables interruptions for time updates
 * @return Returns 0 upong success and non-zero otherwise
 */
int disable_int_time_updates();

/**
 * @brief Gets the day of the week
 * @return Day of the week
 */
uint8_t get_week_day();

/**
 * @brief Gets the hour of the day
 * @return Hour of the day
 */
uint8_t get_hour();

/**
 * @brief Gets the minute
 * @return Current minute
 */
uint8_t get_minute();

/**
 * @brief Gets the second
 * @return Current second
 */
uint8_t get_second();

/**
 * @brief Gets the year
 * @return Current year
 */
uint8_t get_year();

/**
 * @brief Gets the month
 * @return Current month
 */
uint8_t get_month();

/**
 * @brief Gets the day of the month
 * @return Current day of month
 */
uint8_t get_day();

/**
 * @brief Reads the time and date and updates the variables
 * @return Returns 0 upong success and non-zero otherwise
 */
int read_time();

/**
 * @brief Handles an interrupt and updates the variables accordingly
 */
void rtc_int_handler();

#endif
