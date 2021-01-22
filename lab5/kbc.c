#include <lcom/lcf.h>
#include "kbc.h"
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
