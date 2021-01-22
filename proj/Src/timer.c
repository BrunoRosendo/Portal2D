#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "i8254.h"
#include "timer.h"

static int hook_id = TIMER0_IRQ;

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
