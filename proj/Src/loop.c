#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include "loop.h"

int start_loop(char resources_path[]){

  set_resources_path(resources_path);

  if (vg_init(SET_XPM_5_6_5) == NULL) {
    printf("vg_init failed\n");
    return 1;
  }

  if (timer_set_frequency(0, game_freq) != OK)
    return 1;

  if (mouse_enable_data_reporting2() != OK)
    return 1;

  if (timer_subscribe_int(&bit_no_timer) != OK) {
    printf("timer_subscribe_int failed\n");
    return 1;
  }

  if (kbc_subscribe_int(&bit_no_kbc, &hook_id_kbc) != OK) {
    printf("kbc_subscribe_int failed\n");
    return 1;
  }

  if (mouse_subscribe_int(&bit_no_mouse, &hook_id_mouse) != OK) {
    printf("mouse_subscribe_int failed\n");
    return 1;
  }

  if (rtc_subscribe_int(&bit_no_rtc, &hook_id_rtc) != OK){
    printf("rtc_subscribe_int failed\n");
    return 1;
  }

  if (uart_init(&bit_no_uart, &hook_id_uart) != OK){
    printf("Uart_init failed\n");
    return 1;
  }

  if (enable_int_time_updates() != OK){
    printf("enable int updates failed\n");
    return 1;
  }

  start_font();
  if (read_time() != OK) return 1; // initialize variables

  if (start_menu() != OK) return 1;

  if (game_init() != OK) return 1;

  return 0;
}

int main_loop(){
  int ipc_status;
  message msg;
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_kbc = BIT(bit_no_kbc);
  uint32_t irq_set_mouse = BIT(bit_no_mouse);
  uint32_t irq_set_rtc = BIT(bit_no_rtc);
  uint32_t irq_set_uart = BIT(bit_no_uart);
  uint8_t kbc_data = 0;

  while (get_game_state() != OVER) {
    // Get a request message.
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { // received notification
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: // hardware interrupt notification

          // timer
          if (msg.m_notify.interrupts & irq_set_timer) {
            draw_new_frame();
          }

          // keyboard
          if (msg.m_notify.interrupts & irq_set_kbc) {
            kbc_int_handler(&kbc_data);
            key_event_handler(&kbc_data);
          }

          // mouse
          if (msg.m_notify.interrupts & irq_set_mouse) {
            mouse_event ev;
            int16_t x_mov, y_mov;
            if (mouse_int_handler(&ev, &x_mov, &y_mov))
              mouse_event_handler(ev, x_mov, y_mov);
          }

          // RTC
          if (msg.m_notify.interrupts & irq_set_rtc){
            rtc_int_handler();
          }

          // UART
          if ((msg.m_notify.interrupts & irq_set_uart)) {
            if (get_game_state() == COOP) {
              uint8_t data[10];
              if (uart_int_handler(data))
                uart_event_handler(data);
            }
            else
              stash_RBR();
          }

          break;
        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
           // no standard messages expected: do nothing
    }
  }

  return 0;
}

int end_loop(){
  
  if (game_end() != OK) return 1;
  
  if (kbc_unsubscribe_int(&hook_id_kbc) != OK)
    return 1;

  if (mouse_unsubscribe_int(&hook_id_mouse) != OK)
    return 1;
  
  if (timer_unsubscribe_int() != OK)
    return 1;
  
  if (timer_set_frequency(0, 60) != OK)
    return 1; // resets the timer's frequency
  
  if (mouse_disable_data_reporting() != OK)
    return 1;
  
  if (disable_int_time_updates() != OK){
    printf("disable int updates failed\n");
    return 1;
  }

  if (rtc_unsubscribe_int(&hook_id_rtc) != OK) return 1;

  if (uart_end(&hook_id_uart) != OK) return 1;

  end_font();

  if (vg_exit_program() != OK) {
    printf("vg_exit failed\n");
    return 1;
  }

  return 0;
}
