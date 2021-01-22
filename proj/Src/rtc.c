#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include "rtc.h"

int rtc_subscribe_int(uint8_t *bit_no, int *hook_id) {
  *bit_no = *hook_id = RTC_IRQ;
  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, hook_id) != OK) {
    printf("RTC subscription failed\n");
    return 1;
  }
  return 0;
}

int rtc_unsubscribe_int(int *hook_id) {
  if (sys_irqrmpolicy(hook_id) != OK) {
    printf("RTC unsubscription failed\n");
    return 1;
  }
  return 0;
}

int rtc_read_reg(uint8_t reg, uint8_t *byte) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    printf("Out rtc reg failed\n");
    return 1;
  }
  if (util_sys_inb(RTC_DATA_REG, byte) != OK) {
    printf("Read rtc reg failed\n");
    return 1;
  }
  return 0;
}

int rtc_write_reg(uint8_t reg, uint8_t byte) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    printf("Out rtc reg failed\n");
    return 1;
  }
  if (sys_outb(RTC_DATA_REG, byte) != OK) {
    printf("Out rtc cmd failed\n");
    return 1;
  }
  return 0;
}

int enable_int_time_updates() {
  uint8_t b_stat;
  if (rtc_read_reg(REG_B, &b_stat) != OK)
    return 1;

  b_stat = b_stat | SET_UPDATES;

  if (rtc_write_reg(REG_B, b_stat) != OK)
    return 1;
  return 0;
}

int disable_int_time_updates() {
  uint8_t b_stat;
  if (rtc_read_reg(REG_B, &b_stat) != OK)
    return 1;

  b_stat = b_stat & (~SET_UPDATES);

  if (rtc_write_reg(REG_B, b_stat) != OK)
    return 1;
  return 0;
}

uint8_t get_week_day() {
  return week_day;
}

uint8_t get_hour() {
  return hours;
}

uint8_t get_minute() {
  return minutes;
}

uint8_t get_second() {
  return seconds;
}

uint8_t get_year() {
  return year;
}

uint8_t get_month() {
  return month;
}

uint8_t get_day() {
  return day;
}

int read_time() {
  if (rtc_read_reg(REG_SECS, &seconds) != OK)
    return 1;
  if (rtc_read_reg(REG_MINS, &minutes) != OK)
    return 1;
  if (rtc_read_reg(REG_HRS, &hours) != OK)
    return 1;
  if (rtc_read_reg(REG_WEEK_DAY, &week_day) != OK)
    return 1;
  if (rtc_read_reg(REG_MONTH_DAY, &day) != OK)
    return 1;
  if (rtc_read_reg(REG_MONTH, &month) != OK)
    return 1;
  if (rtc_read_reg(REG_YEAR, &year) != OK)
    return 1;
  return 0;
}

void rtc_int_handler() {
  uint8_t data;
  if (rtc_read_reg(REG_C, &data) != OK){
      printf("Read C failed\n");
      return;
  }

  if (data & UPDATE_PENDING){  // we're only interested in updating time
    if (read_time() != OK) printf("Time update failed\n");
  }
}
