#include <lcom/lcf.h>
#include "kbc.h"
#include "keyboard_ih.h"
#include <stdbool.h>
#include <stdint.h>

// File with the reusable functions

int subscribe_kbc(uint8_t *bit_no, int* hook_id){
    *bit_no =  *hook_id = KBC_IRQ;
    if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) != OK){
        printf("KFC subscription failed\n");
        return 1;
    }
    return 0;
}

int unsubscribe_kbc(int *hook_id){
    if (sys_irqrmpolicy(hook_id) != OK){
        printf("KBC unsubscription failed\n");
        return 1;
    }
    return 0;
}

int enable_interrupts(){
    while (1){
        uint8_t stat;
        if(kbc_sys_inb(KBC_ST_REG, &stat)) return 1;
        if ((stat & KBC_IBF) == 0){
            uint8_t cmd;
            if (sys_outb(KBC_CMD_REG, 0X20)){   // Enables read command byte
                printf("Command failed\n");
                return 1;
            }
            if (kbc_sys_inb(KBC_OUT_BUF, &cmd)) return 1; // Reads command byte
            if (sys_outb(KBC_CMD_REG, 0x60)){   // Enables write command byte
                printf("Command failed\n");
                return 1;
            }
            cmd = cmd | BIT(0); // Changes the command to enable interrupts
            if (sys_outb(KBC_ARG_REG, cmd)){ // Enables the interrupts
                printf("Command failed\n");
                return 1;
            }
            return 0;
        }
        else tickdelay(micros_to_ticks(DELAY_US));
    }
}
