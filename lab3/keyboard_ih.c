#include "kbc.h"
#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

// File with specific functions and global variables

uint32_t counter_sys = 0; // global variable to count the number of calls to sys_inb()
uint8_t data;         // use this variable to transfer data from Output Buffer
bool valid = 1;       //use this to know if kbc_ih() read valid data
extern unsigned long counter;

// Use this instead of sys_inb(). It automatically increases the counter value
int kbc_sys_inb(int port, uint8_t *value) {
  uint32_t v = (uint32_t) * (value);
  int ret = sys_inb(port, &v);
  *value = (uint8_t) v;
  counter_sys++;
  return ret;
}

void(kbc_ih)() {
  uint8_t stat;
  if (kbc_sys_inb(KBC_ST_REG, &stat) != OK) {
    printf("Failed at reading status register\n");
    valid = false;
    return;
  }
  if (stat & KBC_OBF) {
    if (kbc_sys_inb(KBC_OUT_BUF, &data) != OK) {
      printf("Failed at reading OBF\n");
      valid = false;
      return;
    }
    if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0)
      valid = true;
    else
      valid = false;
  }
  else
    valid = false;
}

int kbc_int_cycle(uint8_t bit_no) {
  int ipc_status;
  message msg;
  uint32_t irq_set = BIT(bit_no);

  uint8_t d1;
  bool snd_read = false;

  while (data != ESC_KEY) { /* You may want to use a different condition */
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
            kbc_ih();
            if (valid) {
              if (snd_read) {
                uint8_t size = 2;
                uint8_t bytes[2] = {d1, data};
                bool make = !(data & BIT(7));
                if (kbd_print_scancode(make, size, bytes) != OK) {
                  printf("kbd_print_scancode failed\n");
                  return 1;
                }
                snd_read = false;
              }
              else if (data == SBYTE_CODE) {
                d1 = data;
                snd_read = true;
              }
              else {
                bool make = !(data & BIT(7));
                uint8_t size = 1;
                uint8_t bytes[1] = {data};
                if (kbd_print_scancode(make, size, bytes) != OK) {
                  printf("kbd_print_scancode failed\n");
                  return 1;
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

int kbc_poll_cycle() {
  bool snd_read = false;
  uint8_t d1;
  while (data != ESC_KEY) {
    uint8_t stat;
    if (kbc_sys_inb(KBC_ST_REG, &stat))
      return 1;
    if (stat & KBC_OBF) {
      if (kbc_sys_inb(KBC_OUT_BUF, &data))
        return 1;
      if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) == 0) {
        if (snd_read) {
          uint8_t size = 2;
          uint8_t bytes[2] = {d1, data};
          bool make = !(data & BIT(7));
          if (kbd_print_scancode(make, size, bytes) != OK) {
            printf("kbd_print_scancode failed\n");
            return 1;
          }
          snd_read = false;
        }
        else if (data == SBYTE_CODE) {
          snd_read = true;
          d1 = data;
        }
        else {
          uint8_t size = 1;
          uint8_t bytes[1] = {data};
          bool make = !(data & BIT(7));
          if (kbd_print_scancode(make, size, bytes) != OK) {
            printf("kbd_print_scancode failed\n");
            return 1;
          }
        }
      }
    }
    else {
      tickdelay(micros_to_ticks(DELAY_US));
    }
  }
  return 0;
}

int kbd_timed_scan(uint8_t n, uint8_t bit_no_kbd, uint8_t bit_no_timer) {

  int ipc_status, r;
  message msg;
  bool snd_byte = false; // false 1 byte; true 2 bytes
  uint8_t d1;
  uint32_t irq_set_timer = BIT(bit_no_timer);
  uint32_t irq_set_kbd = BIT(bit_no_kbd);

  while (data != ESC_KEY && counter / 60 < n) {
    data = 0;
    /*You may want to use a different condition
        Get a request message.*/
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }

    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set_kbd) { /* subscribed keyboard interrupt */
            kbc_ih();                                 //Handles interrupts in C
            counter = 0;
            if (valid) {
              // if receives second byte
              if (snd_byte) {
                uint8_t size = 2;
                uint8_t bytes[2] = {d1, data};
                // check byte msb to validate if makecode or if breakcode
                bool make = !(data & BIT(7));
                if (kbd_print_scancode(make, size, bytes) != OK) {
                  printf("kbd_print_scancode failed\n");
                  return 1;
                }
                snd_byte = false;
              }
              // if receives the first byte of two bytes scancode
              else if (data == SBYTE_CODE) {
                d1 = data;
                snd_byte = true;
              }
              // one byte scancode
              else {
                bool make = !(data & BIT(7));
                uint8_t size = 1;
                uint8_t bytes[1] = {data};
                if (kbd_print_scancode(make, size, bytes) != OK) {
                  printf("kbd_print_scancode failed\n");
                  return 1;
                }
              }
            }
          }
          if (msg.m_notify.interrupts & irq_set_timer) { /* subscribed interrupt */
            timer_int_handler();                             //Counting the number of interruptions
            continue;                                     //We continue to the next iteration because we don't have any scancode
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
  }
  return 0;
}
