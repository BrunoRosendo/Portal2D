#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

unsigned long counter = 0;
int hook_id = TIMER0_IRQ;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t st;
  timer_get_conf(timer, &st);
  st = 0x0F & st;
  uint8_t rb;
  switch (timer){ // seleciona o timer certo e ativa LSB followed by MSB
    case 0:
      rb = TIMER_SEL0 | TIMER_LSB_MSB | st;
      break;
    case 1:
      rb = TIMER_SEL1 | TIMER_LSB_MSB | st;
      break;
    case 2:
      rb = TIMER_SEL2 | TIMER_LSB_MSB | st;
      break;
    default:
      return 1;
  }
  if (sys_outb(TIMER_CTRL, rb) != OK){
    printf("sys_outb failed\n");
    return 1;
  }
  // converts the frequency
  uint8_t lsb, msb;
  uint16_t counter = TIMER_FREQ/freq; //Isto falha quando o counter nao cabe em 16 bits. É suposto??
  if (util_get_LSB(counter, &lsb) != OK){
    printf("Failed at getting LSB\n");
    return 1;
  }
  if (util_get_MSB(counter, &msb) != OK){
    printf("Failed at getting MSB\n");
    return 1;
  }
  // updates the frequency
  int lsb_res, msb_res;
  switch (timer){
    case 0:
      lsb_res = sys_outb(TIMER_0, lsb);
      msb_res = sys_outb(TIMER_0, msb);
      if (lsb_res || msb_res) return 1;
      break;
    case 1:
      lsb_res = sys_outb(TIMER_1, lsb);
      msb_res = sys_outb(TIMER_1, msb);
      if (lsb_res || msb_res) return 1;
      break;
    case 2:
      lsb_res = sys_outb(TIMER_2, lsb);
      msb_res = sys_outb(TIMER_2, msb);
      if (lsb_res || msb_res) return 1;
      break;
    default:
      return 1;
  }
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK){
    printf("IRQSetPolicy failed\n");
    return 1;
  }
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK){
    printf("IRQ unsubscribing failed\n");
    return 1;
  }
  return 0; 
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t rb = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, rb) != OK){
      printf("sys_outb failed\n");
      return 1;
  }
  switch (timer){
    case 0:
      if (util_sys_inb(TIMER_0, st)) return 1;
      break;
    case 1:
      if (util_sys_inb(TIMER_1, st)) return 1;
      break;
    case 2:
      if (util_sys_inb(TIMER_2, st)) return 1;
      break;
    default:
      return 1;
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) { //st is the timer's byte
  union timer_status_field_val conf;
  switch (field){
    case tsf_all:
      conf.byte = st;
      break;
    case tsf_initial:
      st = st & TIMER_LSB_MSB;
      if (st == TIMER_LSB_MSB) conf.in_mode = MSB_after_LSB;
      else if (st == TIMER_LSB) conf.in_mode = LSB_only;
      else if (st == TIMER_MSB) conf.in_mode = MSB_only;
      else conf.in_mode = INVAL_val;
      break;
    case tsf_mode:
      st = st & (BIT(1) | BIT(2) | BIT(3));
      if (st == TIMER_SQR_WAVE) conf.count_mode = 3;
      else if (st == (BIT(1) | BIT(2) | BIT(3))) conf.count_mode = 3;
      else if (st == TIMER_RATE_GEN) conf.count_mode = 2;
      else if (st == (BIT(2) | BIT(3))) conf.count_mode = 2;
      else if (st == (BIT(3) | BIT(1))) conf.count_mode = 5;
      else if (st == BIT(3)) conf.count_mode = 4;
      else if (st == BIT(1)) conf.count_mode = 1;
      else conf.count_mode = 0;
      break;
    case tsf_base:
      if (TIMER_BCD & st) conf.bcd = true;
      else conf.bcd = false;
      break;
    default:
      return 1;
  }
  return timer_print_config(timer, field, conf);
}
