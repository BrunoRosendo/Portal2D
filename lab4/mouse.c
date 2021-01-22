#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "PS2.h"
#include "i8254.h"
#include "kbc.h"

static uint8_t byte;
static int num_bytes = 0;
static bool valid;

extern unsigned long counter;
extern int hook_id;

int mouse_int_cycle(uint8_t bit_no, uint32_t cnt){
  uint32_t count = 0;
  num_bytes = 0;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  uint8_t bytes[3];


  while (count < cnt) { /* You may want to use a different condition */
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
            mouse_ih();
            if (valid){
                bytes[num_bytes-1] = byte;
                if (num_bytes == 3){
                    count++;
                    num_bytes = 0;
                    struct packet pp;
                    construct_pp(&pp, bytes);
                    mouse_print_packet(&pp);
                }
            }
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
  return 0;
}

void construct_pp(struct packet *pp, uint8_t bytes[]){
    pp->bytes[0] = bytes[0];
    pp->bytes[1] = bytes[1];
    pp->bytes[2] = bytes[2];
    pp->rb = bytes[0] & PS2_RB;
    pp->lb = bytes[0] & PS2_LB;
    pp->mb = bytes[0] & PS2_MB;
    pp->x_ov = bytes[0] & PS2_XOV;
    pp->y_ov = bytes[0] & PS2_YOV;
    bool delta_x_msb = (bytes[0] & PS2_XDELTA_MSB);
    bool delta_y_msb = (bytes[0] & PS2_YDELTA_MSB);
    if (delta_x_msb){
      pp->delta_x = (int16_t) bytes[1] - 256;
    }
    else{
      pp->delta_x = (int16_t) bytes[1];
    }
    if (delta_y_msb){
      pp->delta_y = (int16_t) bytes[2] - 256;
    }
    else{
      pp->delta_y = (int16_t) bytes[2];
    }
    //pp->delta_x = (int16_t) ( ((int16_t) delta_x_msb) << 8) | bytes[1]; //something weird going on
    //pp->delta_y = ( ((int16_t) delta_y_msb) << 8) | bytes[2];
}

void (mouse_ih)(){
    if (read_out_buffer(&byte) != OK){
        valid = false;
        return;
    }
    if (num_bytes == 0){
        if ((byte & BIT(3)) == 0){
            valid = false;
            return;
        }
    }
    num_bytes++;
    valid = true;
}


int mouse_timer_int_cycle(uint8_t bit_no_mouse, uint8_t bit_no_t0, uint8_t idle_time){
  int ipc_status;
  message msg;
  uint32_t irq_set_mouse = BIT(bit_no_mouse);
  uint32_t irq_set_t0 = BIT(bit_no_t0);
  uint8_t bytes[3];
  num_bytes = 0;

  while (counter / sys_hz() < idle_time) { /* You may want to use a different condition */
    int r;
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                             /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_mouse) { /* subscribed interrupt */
            counter = 0;
            mouse_ih();
            if (valid){
                bytes[num_bytes-1] = byte;
                if (num_bytes == 3){
                    num_bytes = 0;
                    struct packet pp;
                    construct_pp(&pp, bytes);
                    mouse_print_packet(&pp);
                }
            }
          }
          if (msg.m_notify.interrupts & irq_set_t0){
            timer_int_handler();
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
  return 0;
}

int mouse_remote_cycle(uint16_t period, uint8_t cnt){
  int counter = 0;
  uint8_t bytes[3];
  num_bytes = 0;
  while (counter < cnt){
    if (write_mouse_cmd(PS2_READ_DATA) != OK)
      return 1;
    if (kbc_able_to_read()){
      uint8_t byte;
      if (read_out_buffer(&byte)) return 1;
      if (num_bytes == 0){
        if ((byte & BIT(3)) == 0){
          continue;
        }
      }
      bytes[num_bytes] = byte;
      num_bytes++;
      if (num_bytes == 3){
        struct packet pp;
        construct_pp(&pp, bytes);
        mouse_print_packet(&pp);
        counter++;
        num_bytes = 0;
        tickdelay(micros_to_ticks(period * 1000));
      }
    }
  }
  return 0;
}

int mouse_gesture_cycle(uint8_t bit_no, uint8_t x_len, uint8_t tolerance){
  num_bytes = 0;
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);
  uint8_t bytes[3];
  char state = 0;
  int y_mov = 0, x_mov = 0;

  while (state != 4) { /* You may want to use a different condition */
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
            mouse_ih();
            if (valid){
                bytes[num_bytes-1] = byte;
                if (num_bytes == 3){
                    num_bytes = 0;
                    struct packet pp;
                    construct_pp(&pp, bytes);
                    mouse_print_packet(&pp);

                    // check exit condition
                    switch (state){
                      case 0:{
                        y_mov = 0;
                        x_mov = 0;
                        if (pp.mb || pp.rb) break;
                        if (pp.lb){
                          if (pp.delta_x < - tolerance || pp.delta_y < - tolerance) break;
                          y_mov += pp.delta_y;
                          x_mov += pp.delta_x;
                          state = 1;
                        }
                      break;
                      }
                      case 1:{
                        if (pp.mb || pp.rb){
                          state = 0;
                          break;
                        }
                        if (!pp.lb){
                          if ((double) y_mov/x_mov <= 1) state = 0;
                          else state = 2;
                          if (x_mov < x_len) state = 0;
                          break;
                        }
                        if (pp.delta_x < - tolerance || pp.delta_y < - tolerance){
                          state = 0;
                          break;
                        }
                        y_mov += pp.delta_y;
                        x_mov += pp.delta_x;
                        break;
                      }
                      case 2:{
                        y_mov = 0;
                        x_mov = 0;
                        if (pp.lb || pp.mb){
                          state = 0;
                          break;
                        }
                        if (abs(pp.delta_x) > tolerance || abs(pp.delta_y) > tolerance){
                          state = 0;
                          break;
                        }
                        if (pp.rb) state = 3;
                        break;
                      }
                      case 3:{
                        if (pp.mb || pp.lb){
                          state = 0;
                          break;
                        }
                        if (!pp.rb){
                          if (-(double) y_mov/x_mov <= 1) state = 0;
                          else state = 4;
                          if (x_mov < x_len) state = 0;
                          break;
                        }
                        if (pp.delta_x < - tolerance || pp.delta_y > tolerance){
                          state = 0;
                          break;
                        }
                        y_mov += pp.delta_y;
                        x_mov += pp.delta_x;
                        break;
                      }
                      default:
                        break;
                    }

                }
            }
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
  return 0;
}
