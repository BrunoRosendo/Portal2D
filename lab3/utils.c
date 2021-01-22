#include <lcom/lcf.h>

#include <stdint.h>

//Estas duas funçoes get devem estar a dar mas verifica

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) (val >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) { //8 to 32 bits int sys_inb(int port, u32_t *byte);
  uint32_t v = (uint32_t) *(value);
  int ret = sys_inb(port, &v);
  *value = (uint8_t) v;
  return ret;
}
