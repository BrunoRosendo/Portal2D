#include "menu.h"
#include "xpm_files.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int start_menu() {
  background = create_sprite(menu_back, 0, 0, 0, 0);
  exit_but = create_sprite(menu_exit, 875, 780, 0, 0);
  play_but = create_sprite(play, 30, 100, 0, 0);
  coop_but = create_sprite(coop, 430, 100, 0, 0);
  editor_but = create_sprite(editor, 200, 430, 0, 0);
  return 0;
}

int exit_menu() {
  erase_menu();
  destroy_sprite(background);
  destroy_sprite(editor_but);
  destroy_sprite(play_but);
  destroy_sprite(exit_but);
  destroy_sprite(coop_but);
  return 0;
}

void draw_menu(){
  draw_sprite(background);
  draw_sprite(exit_but);
  draw_sprite(play_but);
  draw_sprite(coop_but);
  draw_sprite(editor_but);
  flip_page();
  draw_sprite(background);
  draw_sprite(exit_but);
  draw_sprite(play_but);
  draw_sprite(coop_but);
  draw_sprite(editor_but);
  flip_page();
}

void erase_menu(){
  erase_everything();
  flip_page();
  erase_everything();
}

char check_buttons(int x, int y) {
  if (x > play_but->x && x < play_but->x + play_but->width && y > play_but->y && y < play_but->y + play_but->height)
    return 'p';

  if (x > coop_but->x && x < coop_but->x + coop_but->width && y > coop_but->y && y < coop_but->y + coop_but->height)
    return 'c';

  if (x > editor_but->x && x < editor_but->x + editor_but->width && y > editor_but->y && y < editor_but->y + editor_but->height)
    return 'l';

  if (x > exit_but->x && x < exit_but->x + exit_but->width && y > exit_but->y && y < exit_but->y + exit_but->height)
    return 'e';

  return 0;
}
