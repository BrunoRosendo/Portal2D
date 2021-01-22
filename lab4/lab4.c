// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "PS2.h"
#include "kbc.h"
#include "mouse.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/shared/g07/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/g07/lab4/trace.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

extern int hook_id;

int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no;
    int hook_id;
    if (mouse_enable_data_reporting2() != OK){
      printf("enable_data_reporting failed\n");
      return 1;
    }
    if (mouse_subscribe_int(&bit_no, &hook_id) != OK) return 1;
    if (mouse_int_cycle(bit_no, cnt) != OK) return 1;
    if (mouse_unsubscribe_int(&hook_id) != OK) return 1;
    if (mouse_disable_data_reporting() != OK){
      printf("disable_data_reporting failed\n");
      return 1;
    }
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    uint8_t bit_no_kbd;
    int hook_id_kbd;
    uint8_t bit_no_t0;
    if (mouse_enable_data_reporting2() != OK){
      printf("enable_data_reporting failed\n");
      return 1;
    }
    if (mouse_subscribe_int(&bit_no_kbd, &hook_id_kbd) != OK) return 1;
    if (timer_subscribe_int(&bit_no_t0)) return 1;
    if (mouse_timer_int_cycle(bit_no_kbd, bit_no_t0, idle_time)) return 1;
    if (timer_unsubscribe_int() != OK) return 1;
    if (mouse_unsubscribe_int(&hook_id_kbd) != OK) return 1;
    if (mouse_disable_data_reporting() != OK){
      printf("disable_data_reporting failed\n");
      return 1;
    }
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    uint8_t bit_no;
    int hook_id;
    if (mouse_enable_data_reporting2() != OK){
      printf("enable_data_reporting failed\n");
      return 1;
    }
    if (mouse_subscribe_int(&bit_no, &hook_id) != OK) return 1;
    if (mouse_gesture_cycle(bit_no, x_len, tolerance) != OK) return 1;
    if (mouse_unsubscribe_int(&hook_id) != OK) return 1;
    if (mouse_disable_data_reporting() != OK){
      printf("disable_data_reporting failed\n");
      return 1;
    }
    if (restore_kbc_and_mouse() != OK) return 1;
    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    if (mouse_remote_cycle(period, cnt) != OK) return 1;
    if (restore_kbc_and_mouse() != OK) return 1;
    return 0;
}
