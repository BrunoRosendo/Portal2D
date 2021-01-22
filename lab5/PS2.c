#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "PS2.h"
#include "kbc.h"

int mouse_subscribe_int(uint8_t *bit_no, int* hook_id){
    *bit_no =  *hook_id = PS2_IRQ;
    if (sys_irqsetpolicy(PS2_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) != OK){
        printf("Mouse subscription failed\n");
        return 1;
    }
    return 0;
}

int read_out_buffer(uint8_t* byte){ // mudar pra usar a funçao able_to_read
    int tries = 0;
    while (tries < 4){
        tries++;
        if (kbc_able_to_read()){
            if (util_sys_inb(KBC_OUT_BUF, byte) != OK){
                printf("Failed at reading out buffer\n");
                return 1;
            }
            return 0;
        }
        else{
            tickdelay(micros_to_ticks(DELAY_US));
            continue;
        }
    }
    printf("read_out_buffer Timeout\n");
    return 1;
}

int mouse_unsubscribe_int(int *hook_id){
    if (sys_irqrmpolicy(hook_id) != OK){
        printf("KBC unsubscription failed\n");
        return 1;
    }
    return 0;
}

bool kbc_able_to_write(){
    uint8_t stat;
    if (util_sys_inb(KBC_ST_REG, &stat) != OK) return false;
    if ((stat & KBC_IBF)) return false;
    return true;
}

bool kbc_able_to_read(){
    uint8_t stat;
    if (util_sys_inb(KBC_ST_REG, &stat) != OK) return false;
    if ((stat & KBC_OBF) == 0) return false;    // mouse isn't filling the output buffer when testing manually, idk why
    if ((stat & (KBC_PAR_ERR | KBC_TO_ERR)) != OK){
        printf("Error in Out buffer\n");
        return false;
    }
    return true;
}

int kbc_write_cmd(uint8_t cmd){ // use this for PS2_WRITE_BYTE
    int tries = 0;
    while (tries < 4){  // 4 tries only
        tries++;
        if (kbc_able_to_write()){
            if (sys_outb(KBC_CMD_REG, cmd) != OK){
                printf("Failed to write byte command\n");
                return 1;
            }
            return 0;
        }
        else tickdelay(micros_to_ticks(DELAY_US));
    }
    printf("kbc_write_cmd Timeout\n");
    return 1;
}

int kbc_write_arg(uint8_t arg){
    int tries = 0;
    while (tries < 4){
        tries++;
        if (kbc_able_to_write()){
            if (sys_outb(KBC_ARG_REG, arg) != OK){
                printf("Failed to write arg\n");
                return 1;
            }
            return 0;
        }
        else tickdelay(micros_to_ticks(DELAY_US));
    }
    printf("kbc_write_arg Timeout\n");
    return 1;
}


int write_mouse_cmd(uint8_t cmd){   // mouse cmd is actually kbc arg
    uint8_t ack;
    int tries = 0;
    while (tries < 4){
        if (kbc_write_cmd(PS2_WRITE_BYTE) != OK) return 1;
        if (kbc_write_arg(cmd) != OK) return 1;
        tries++;
        if (read_out_buffer(&ack) != OK){
             tickdelay(micros_to_ticks(DELAY_US));
             continue;
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
