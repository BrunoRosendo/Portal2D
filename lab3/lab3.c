#include <lcom/lcf.h>
#include "kbc.h"
#include "keyboard_ih.h"
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "timer.h"

extern uint32_t counter_sys;
extern uint8_t data;
extern bool valid;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/g07/lab3/trace.txt"); // CHANGE THE PATH!!!!!

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/g07/lab3/trace.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  uint8_t bit_no;
  int hook_id;

  if (subscribe_kbc(&bit_no, &hook_id) != OK) return 1;

  if (kbc_int_cycle(bit_no)) return 1;

  if (kbd_print_no_sysinb(counter_sys) != OK){
    printf("kbd_print_no_sysinb failed\n");
    return 1;
  }
  if (unsubscribe_kbc(&hook_id) != OK) return 1;
  return 0;
}

int(kbd_test_poll)() {
  if (kbc_poll_cycle()) return 1;
  if (kbd_print_no_sysinb(counter_sys) != OK){
    printf("kbd_print_no_sysinb failed\n");
    return 1;
  }
  if (enable_interrupts() != OK) return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t bit_no_kbd;
  int hook_id;

  if (subscribe_kbc(&bit_no_kbd, &hook_id) != OK) return 1;

  uint8_t bit_no_timer;
  if (timer_subscribe_int(&bit_no_timer) != OK) return 1;

  kbd_timed_scan(n, bit_no_kbd, bit_no_timer);

  if (unsubscribe_kbc(&hook_id) != OK) return 1;

  if (timer_unsubscribe_int() != OK) return 1;
  return 0;
}
