#include "font.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

void start_font() {
  numbers[0] = create_sprite(zero, 0, 0, 0, 0);
  numbers[1] = create_sprite(one, 0, 0, 0, 0);
  numbers[2] = create_sprite(two, 0, 0, 0, 0);
  numbers[3] = create_sprite(three, 0, 0, 0, 0);
  numbers[4] = create_sprite(four, 0, 0, 0, 0);
  numbers[5] = create_sprite(five, 0, 0, 0, 0);
  numbers[6] = create_sprite(six, 0, 0, 0, 0);
  numbers[7] = create_sprite(seven, 0, 0, 0, 0);
  numbers[8] = create_sprite(eight, 0, 0, 0, 0);
  numbers[9] = create_sprite(nine, 0, 0, 0, 0);

  week_days[0][0] = create_sprite(big_s, 0, 0, 0, 0);
  week_days[0][1] = create_sprite(small_u, 0, 0, 0, 0);
  week_days[0][2] = create_sprite(small_n, 0, 0, 0, 0);
  week_days[1][0] = create_sprite(big_m, 0, 0, 0, 0);
  week_days[1][1] = create_sprite(small_o, 0, 0, 0, 0);
  week_days[1][2] = create_sprite(small_n, 0, 0, 0, 0);
  week_days[2][0] = create_sprite(big_t, 0, 0, 0, 0);
  week_days[2][1] = create_sprite(small_u, 0, 0, 0, 0);
  week_days[2][2] = create_sprite(small_e, 0, 0, 0, 0);
  week_days[3][0] = create_sprite(big_w, 0, 0, 0, 0);
  week_days[3][1] = create_sprite(small_e, 0, 0, 0, 0);
  week_days[3][2] = create_sprite(small_d, 0, 0, 0, 0);
  week_days[4][0] = create_sprite(big_t, 0, 0, 0, 0);
  week_days[4][1] = create_sprite(small_h, 0, 0, 0, 0);
  week_days[4][2] = create_sprite(small_u, 0, 0, 0, 0);
  week_days[5][0] = create_sprite(big_f, 0, 0, 0, 0);
  week_days[5][1] = create_sprite(small_r, 0, 0, 0, 0);
  week_days[5][2] = create_sprite(small_i, 0, 0, 0, 0);
  week_days[6][0] = create_sprite(big_s, 0, 0, 0, 0);
  week_days[6][1] = create_sprite(small_a, 0, 0, 0, 0);
  week_days[6][2] = create_sprite(small_t, 0, 0, 0, 0);

  two_dots = create_sprite(twodots, 0, 0, 0, 0);
  slash_date = create_sprite(slash, 0, 0, 0, 0);
}

void end_font() {
  for (int i = 0; i < 10; ++i)
    destroy_sprite(numbers[i]);

  for (int i = 0; i < 7; ++i) {
    for (int j = 0; j < 3; ++j)
      destroy_sprite(week_days[i][j]);
  }
}

void get_pair_of_digits(uint8_t num, int x, int y, Sprite *first, Sprite *second) {
  uint8_t first_num = (num & GET_FIRST) >> 4;
  uint8_t second_num = num & GET_SECOND;
  
  numbers[first_num]->x = x;
  numbers[first_num]->y = y;
  *first = *numbers[first_num];

  numbers[second_num]->x = x + 27;
  numbers[second_num]->y = y;
  
  *second = *numbers[second_num];
}

void get_week_day_sprite(uint8_t day, int x, int y, Sprite *week_day[]) {
  if (day == 7)
    day = 0;
  for (int i = 0; i < 3; ++i)
    week_day[i] = week_days[day][i];
  week_day[0]->x = x;
  week_day[0]->y = y;
  week_day[1]->x = x + week_day[0]->x + 6;
  week_day[1]->y = y;
  week_day[2]->x = x + week_day[1]->x + 1;
  week_day[2]->y = y;
}

Sprite *get_dots(int x, int y) {
  two_dots->x = x;
  two_dots->y = y;
  return two_dots;
}

Sprite *get_slash(int x, int y) {
  slash_date->x = x;
  slash_date->y = y;
  return slash_date;
}

void draw_time(Sprite old_sprites[]){ // mudar pra Sprite e nao pointer
  Sprite* week_sprites[3];
  Sprite hr1, hr2, *p1, min1, min2, *p2, sec1, sec2;
  Sprite d1, d2, *b1, m1, m2, *b2, y1, y2;
  
  get_week_day_sprite(get_week_day(), 20, 5, week_sprites);
  for (int i = 0; i < 3; ++i){
    draw_sprite(week_sprites[i]);
    old_sprites[i] = *week_sprites[i];
  }

  get_pair_of_digits(get_hour(), 105, 10, &hr1, &hr2);
  draw_sprite(&hr1);
  draw_sprite(&hr2);
  old_sprites[3] = hr1;
  old_sprites[4] = hr2;

  p1 = get_dots(163, 10);
  draw_sprite(p1);
  old_sprites[5] = *p1;

  get_pair_of_digits(get_minute(), 181, 10, &min1, &min2);
  draw_sprite(&min1);
  draw_sprite(&min2);
  old_sprites[6] = min1;
  old_sprites[7] = min2;

  p2 = get_dots(231, 10);
  draw_sprite(p2);
  old_sprites[8] = *p2;

  get_pair_of_digits(get_second(), 247, 10, &sec1, &sec2);
  draw_sprite(&sec1);
  draw_sprite(&sec2);
  old_sprites[9] = sec1;
  old_sprites[10] = sec2;

  get_pair_of_digits(get_day(), 1060, 10, &d1, &d2);
  draw_sprite(&d1);
  draw_sprite(&d2);
  old_sprites[11] = d1;
  old_sprites[12] = d2;

  b1 = get_slash(1112, 10);
  draw_sprite(b1);
  old_sprites[13] = *b1;

  get_pair_of_digits(get_month(), 1130, 10, &m1, &m2);
  draw_sprite(&m1);
  draw_sprite(&m2);
  old_sprites[14] = m1;
  old_sprites[15] = m2;

  b2 = get_slash(1183, 10);
  draw_sprite(b2);
  old_sprites[16] = *b2;

  get_pair_of_digits(get_year(), 1200, 10, &y1, &y2);
  draw_sprite(&y1);
  draw_sprite(&y2);
  old_sprites[17] = y1;
  old_sprites[18] = y2;
}
