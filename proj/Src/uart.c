#include "uart.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

int uart_init(uint8_t *bit_no, int *hook_id) {
  if (uart_subscribe_int(bit_no, hook_id) != OK)
    return 1;
  if (uart_enable_interrupts() != OK)
    return 1;
  if (uart_set_bitrate(9600) != OK)
    return 1;
  if (uart_config(8, 1, LCR_ODD_PAR) != OK)
    return 1;

   // throw garbage away
  stash_RBR();

  return 0;
}

int uart_end(int *hook_id) {
  if (uart_disable_interrupts() != OK)
    return 1;
  if (uart_unsubscribe_int(hook_id) != OK)
    return 1;
  return 0;
}

int uart_subscribe_int(uint8_t *bit_no, int *hook_id) {
  *bit_no = *hook_id = COM1_IRQ;
  if (sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, hook_id) != OK) {
    printf("UART Subscribe Int failed\n");
    return 1;
  }
  return 0;
}

int uart_unsubscribe_int(int *hook_id) {
  if (sys_irqrmpolicy(hook_id) != OK) {
    printf("UART Unsubscribe Int failed\n");
    return 1;
  }
  return 0;
}

int uart_read_reg(int reg_offset, uint8_t *data) {
  if (util_sys_inb(COM1_BASE_ADDR + reg_offset, data) != OK) {
    printf("UART Read failed\n");
    return 1;
  }
  return 0;
}

int uart_write_reg(int reg_offset, uint8_t data) {
  if (sys_outb(COM1_BASE_ADDR + reg_offset, data) != OK) {
    printf("UART Write failed\n");
    return 1;
  }
  return 0;
}

int uart_enable_interrupts() {
  if (uart_write_reg(REG_IER, IER_DATA) != OK) {
    printf("Data int enable failed\n");
    return 1;
  }
  return 0;
}

int uart_disable_interrupts() {
  if (uart_write_reg(REG_IER, 0) != OK) {
    printf("Int disable failed\n");
    return 1;
  }
  return 0;
}

int uart_enable_dlab() {
  uint8_t lcr_stat;
  if (uart_read_reg(REG_LCR, &lcr_stat) != OK)
    return 1;
  lcr_stat |= LCR_DLAB;
  if (uart_write_reg(REG_LCR, lcr_stat) != OK)
    return 1;
  return 0;
}

int uart_disable_dlab() {
  uint8_t lcr_stat;
  if (uart_read_reg(REG_LCR, &lcr_stat) != OK)
    return 1;
  lcr_stat &= (~LCR_DLAB);
  if (uart_write_reg(REG_LCR, lcr_stat) != OK)
    return 1;
  return 0;
}

int uart_set_bitrate(unsigned int bitrate) {
  uint16_t reg_rate = BIT_RATE_DIV / bitrate;
  uint8_t lsb, msb;
  util_get_LSB(reg_rate, &lsb);
  util_get_MSB(reg_rate, &msb);

  if (uart_enable_dlab() != OK) {
    printf("Dlab enable failed\n");
    return 1;
  }

  if (uart_write_reg(REG_DLL, lsb) != OK) {
    printf("Bitrate LSB failed\n");
    return 1;
  }

  if (uart_write_reg(REG_DLM, msb) != OK) {
    printf("Bitrate MSB failed\n");
    return 1;
  }

  if (uart_disable_dlab() != OK) {
    printf("Dlab disaable failed\n");
    return 1;
  }
  return 0;
}

int uart_config(uint8_t bpc, uint8_t stop_bits, uint8_t par) {
  uint8_t lcr_stat;
  if (uart_read_reg(REG_LCR, &lcr_stat) != OK)
    return 1;

  // bits per character

  if (bpc < 5 || bpc > 8) {
    printf("Invalid bpc\n");
    return 1;
  }
  lcr_stat &= ~(LCR_BPC); // zero the first 2 bits
  lcr_stat |= (bpc - 5);

  // stop_bits

  if (stop_bits < 1 || stop_bits > 2) {
    printf("Invalid stop_bits\n");
    return 1;
  }
  if (stop_bits == 1)
    lcr_stat |= LCR_1SP;
  else
    lcr_stat |= LCR_2SP;

  // Parity

  if (par < (LCR_NO_PAR) || par > (LCR_PAR_IS_0)) {
    printf("Invalid Parity\n");
    return 1;
  }
  lcr_stat |= par;

  if (uart_write_reg(REG_LCR, lcr_stat) != OK)
    return 1;

  return 0;
}

int uart_write_char(uint8_t byte) {
  int tries = 0;

  while (tries < 4) {
    uint8_t lsr_stat;
    if (uart_read_reg(REG_LSR, &lsr_stat) != OK)
      return 1;

    if (lsr_stat & LSR_THRE) {
      if (uart_write_reg(REG_THR, byte) != OK)
        return 1;
      return 0;
    }
    else {
      tries++;
      tickdelay(micros_to_ticks(P_DELAY));
    }
  }
  printf("UART Send timeout\n");
  return 1;
}

int uart_write_message(uint8_t message[], int size) {
  for (int i = 0; i < size; ++i) {
    if (uart_write_char(message[i]) != OK)
      return 1;

    uint8_t ack;
    if (uart_read_ack(&ack) != OK)
      return 1;
    
    if (ack != P_ACK) {
      printf("Byte not acknowledged\n");
      stash_RBR();
      return 1;
    }
  }

  if (uart_write_char(P_MESSAGE_END) != OK)
    return 1;

  uint8_t ack;
  if (uart_read_ack(&ack) != OK)
    return 1;

  if (ack != P_ACK) {
    printf("Byte not acknowledged\n");
    stash_RBR();
    return 1;
  }

  return 0;
}

int uart_read_ack(uint8_t* byte){
  int tries = 0;

  while (tries < 4){
    uint8_t lsr_stat;
    if (uart_read_reg(REG_LSR, &lsr_stat) != OK)
      return 1;
    
    if (lsr_stat & LSR_REC_DATA){
      if (uart_read_reg(REG_RBR, byte) != OK) return 1;
      return 0;
    }
    else{
        tries++;
        tickdelay(micros_to_ticks(P_DELAY));
    }
  }
  printf("Ack timeout\n");
  stash_RBR();
  return 1;
}

bool uart_read_data(uint8_t data[]){
  uint8_t lsr_stat;
  if (uart_read_reg(REG_LSR, &lsr_stat) != OK)
    return false;
  
  if (lsr_stat & LSR_REC_DATA){

    //check if there's an error first
    if (lsr_stat & (LSR_FRAME_ERR | LSR_PAR_ERR | LSR_OVER_ERR)){
      bytes_count = 0;
      uart_write_char(P_NACK); // send it again
      return false;
    }

    uint8_t byte;
    if (uart_read_reg(REG_RBR, &byte) != OK) return false;

    uart_message[bytes_count] = byte;
    bytes_count++;

    if (uart_write_char(P_ACK) != OK) return false;

    if (byte == P_MESSAGE_END){
      for (int i = 0; i < bytes_count; ++i) data[i] = uart_message[i];
      bytes_count = 0;
      return true;
    }
  }
  return false;
}

bool uart_int_handler(uint8_t data[]){
  return uart_read_data(data);
}

void stash_RBR(){
  while (true) {
    uint8_t lsr_stat;
    if (uart_read_reg(REG_LSR, &lsr_stat) != OK)
      return;

    if (lsr_stat & LSR_REC_DATA) {
      uint8_t byte;
      uart_read_reg(REG_RBR, &byte);
      //printf("%x\n", byte);
    }
    else
      break;
  }
  bytes_count = 0;
}
