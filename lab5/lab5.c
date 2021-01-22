// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "graphics.h"
#include "kbc.h"
#include "PS2.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/shared/g07/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/shared/g07/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(vg_init(mode) == NULL){
    printf("vg_init failed\n");
    return 1;
  }
  sleep(delay);
  if (vg_exit() != OK){
    printf("vg_exit failed\n");
    return 1;
  }
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  uint8_t bit_no;
  int hook_id;
  if (subscribe_kbc(&bit_no, &hook_id) != OK) return 1;

  if (vg_init(mode) == NULL){
    printf("vg_init failed\n");
    return 1;
  }

  if (vg_draw_rectangle(x, y, width, height, color) != OK ) return 1;
  
  // end with keyboard interrupts
  
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  uint8_t data = 0;

  while (data != ESC_KEY) {
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
           if (read_out_buffer(&data) != OK) return 1;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  if (unsubscribe_kbc(&hook_id) != OK) return 1;

  if (vg_exit() != OK){
    printf("vg_exit failed\n");
    return 1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t bit_no;
  int hook_id;
  if (subscribe_kbc(&bit_no, &hook_id) != OK) return 1;

  if (vg_init(mode) == NULL){
    printf("vg_init failed\n");
    return 1;
  }

  if (vg_draw_pattern(no_rectangles, first, step) != OK) return 1;

  // end with keyboard interrupts
  
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  uint8_t data = 0;

  while (data != ESC_KEY) {
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
           if (read_out_buffer(&data) != OK) return 1;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  if (unsubscribe_kbc(&hook_id) != OK) return 1;
  
  if (vg_exit() != OK){
    printf("vg_exit failed\n");
    return 1;
  }

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {

  uint8_t bit_no;
  int hook_id;
  if (subscribe_kbc(&bit_no, &hook_id) != OK) return 1; 

  if (vg_init(SET_INDEXED_MODE) == NULL){
    printf("vg_init failed\n");
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;
  if (vg_draw_xpm(xpm, x, y, type)){
    printf("Error in vg_draw_xpm\n");
    return 1;
  }

  // end with keyboard interrupts
  
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  uint8_t data = 0;

  while (data != ESC_KEY) {
    /* Get a request message. */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
           if (read_out_buffer(&data) != OK) return 1;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
    }
  }

  if (unsubscribe_kbc(&hook_id) != OK) return 1;
  
  if (vg_exit() != OK){
    printf("vg_exit failed\n");
    return 1;
  }

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  if (vg_init(SET_INDEXED_MODE) == NULL){
    printf("vg_init failed\n");
    return 1;
  }

  enum xpm_image_type type = XPM_INDEXED;
  if (animate_xpm(xpm, xi, yi, xf, yf, speed, fr_rate, type) != OK) return 1;
  
  if (vg_exit() != OK){
    printf("vg_exit failed\n");
    return 1;
  }

  return 0;
}

int(video_test_controller)() {  // we're not supposed to do it
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
