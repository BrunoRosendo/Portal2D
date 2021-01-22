#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t v = (uint32_t) *(value);
  int ret = sys_inb(port, &v);
  *value = (uint8_t) v;
  return ret;
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) (val >> 8);
  return 0;
}
