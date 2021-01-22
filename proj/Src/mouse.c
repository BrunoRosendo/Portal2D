#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mouse.h"

int mouse_subscribe_int(uint8_t *bit_no, int* hook_id){
    *bit_no =  *hook_id = PS2_IRQ;
    if (sys_irqsetpolicy(PS2_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) != OK){
        printf("Mouse subscription failed\n");
        return 1;
    }
    return 0;
}

int mouse_unsubscribe_int(int *hook_id){
    if (sys_irqrmpolicy(hook_id) != OK){
        printf("KBC unsubscription failed\n");
        return 1;
    }
    return 0;
}

int write_mouse_cmd(uint8_t cmd){   // mouse cmd is actually kbc arg
    uint8_t ack;
    int tries = 0;
    while (tries < 4){
        if (kbc_write_cmd(PS2_WRITE_BYTE) != OK) return 1;
        if (kbc_write_arg(cmd) != OK) return 1;
        tries++;
        if (read_out_buffer(&ack) != OK){
             return 0;  // that's the only way it works. Please don't ask why. The real mouse just doesn't send an ack
        }
        if (ack == PS2_ACK_OK)
             return 0;
        else if (ack == PS2_NACK) tickdelay(micros_to_ticks(DELAY_US));
        else if (ack == PS2_ERROR){
            printf("Acknowledgment Error\n");
            return 1;
        }
        else tickdelay(micros_to_ticks(DELAY_US));
    }
    printf("Acknowledgment timeout error\n");
    return 1;
}

int mouse_disable_data_reporting(){
    if (write_mouse_cmd(PS2_DIS_DATA)) return 1;
    return 0;
}

int mouse_enable_data_reporting2(){
    if (write_mouse_cmd(PS2_EN_DATA)) return 1;
    return 0;
}

int restore_kbc_and_mouse(){
  if (write_mouse_cmd(PS2_SET_STREAMMODE) != OK) return 1;
  if (mouse_disable_data_reporting() != OK) return 1;
  if (kbc_write_cmd(KBC_WRITE_CMDBYTE) != OK) return 1;
  uint8_t dflt = minix_get_dflt_kbc_cmd_byte();
  if (kbc_write_arg(dflt) != OK) return 1;
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
}

bool mouse_int_handler(mouse_event* event, int16_t* x_mov, int16_t* y_mov){
    uint8_t byte;
    if (read_out_buffer(&byte) != OK){
        return false;
    }
    if (num_bytes == 0){
        if ((byte & BIT(3)) == 0){
            return false;
        }
    }
    bytes[num_bytes] = byte;
    num_bytes++;

    if (num_bytes == 3){
        struct packet pp;
        construct_pp(&pp, bytes);

        (*x_mov) = pp.delta_x;
        (*y_mov) = pp.delta_y;
        
        if (pp.lb) (*event) = MOUSE_LB_PRESSED;
        else if (pp.rb) (*event) = MOUSE_RB_PRESSED;
        else if (pp.delta_x != 0 || pp.delta_y != 0) (*event) = MOUSE_MOVEMENT;
        else (*event) = MOUSE_OTHER_EV;

        num_bytes = 0;
        return true;
    }
    return false;
}
