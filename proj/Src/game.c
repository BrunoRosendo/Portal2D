#include "game.h"
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

int game_init() {

  if (game_state == MAIN_GAME || game_state == COOP) {
    if (read_level_from_file() != OK)
      return 1;
    if (game_state == COOP)
      stash_RBR();
  }

  else if (game_state == LEVEL_EDITOR) {
    if (editor_init() != OK)
      return 1;
  }

  else if (game_state == MENU) {
    draw_menu();
    load_background();
    cursor = create_sprite(cursor_xpm, 600, 500, 0, 0);
    old_cursor = *(cursor);
    draw_sprite(cursor);
    draw_time(old_time);
  }

  buffer_screen();
  flip_page();

  return 0;
}

int game_end() {
  if (game_state == MAIN_GAME || game_state == COOP) {
    current_level = 0;
    frame_counter = 0;
    char_reversed = false;
    char_state = 0;
    char_reversed2 = false;
    char_state2 = 0;
  }
  else if (game_state == LEVEL_EDITOR) {
    if (editor_end() != OK)
      return 1;
  }
  else if (game_state == MENU) {
    erase_menu();
  }
  else if (game_state == OVER)
    exit_menu();

  if (game_state != MENU && game_state != OVER)
    destroy_all_sprites();

  return 0;
}

int read_level_from_file() {

  // check if there are more levels
  char path[] = "/Levels/next.txt";
  char fileLevel[70] = "";
  strcat(fileLevel, resources_path);
  strcat(fileLevel, path);

  FILE *ptr_next;
  if ((ptr_next = fopen(fileLevel, "r")) == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  int l = 0;
  fscanf(ptr_next, "%d", &l);
  if (l == current_level + 1) {
    game_end();
    game_state = MENU;
    game_init();
    return 0;
  }

  if (current_level != 0) destroy_all_sprites();

  // read the next level

  current_level++;
  char file[4];
  char suffix[] = ".txt";
  int level = current_level;
  sprintf(file, "%d", level);
  strcat(file, suffix);

  char levelsPath[] = "/Levels/";
  char filePath[70] = "";
  strcat(filePath, resources_path);
  strcat(filePath, levelsPath);
  strcat(filePath, file);

  FILE *ptr;
  if ((ptr = fopen(filePath, "r")) == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  if (fscanf(ptr, "%d", &block_num) == 0) {
    printf("Error reading file\n");
    return 1;
  }
  blocks = (Sprite **) malloc((block_num) * sizeof(Sprite *));

  int block_index = 0;
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 20; ++j) {
      char type;
      if (fscanf(ptr, "%c", &type) == 0) {
        printf("Error reading file\n");
        return 1;
      }
      switch (type) {

        case ' ': {
          grid[i][j] = 0;
          break;
        }

        case 'w': {
          grid[i][j] = 1;
          blocks[block_index++] = create_sprite(white_block, j * block_width, i * block_width, 0, 0);
          break;
        }

        case 'b': {
          grid[i][j] = 2;
          blocks[block_index++] = create_sprite(black_block, j * block_width, i * block_width, 0, 0);
          break;
        }

        case 's': {
          character = create_sprite(chell, j * block_width, i * block_width, 0, 0);
          xpm_image_t img;
          character->map_reversed = xpm_load(chell_r, get_type(), &img);
          if (game_state == COOP) {
            character2 = create_sprite(chell2, j * block_width, i * block_width, 0, 0);
            character2->map_reversed = xpm_load(chell2_r, get_type(), &img);
          }
          break;
        }

        case 'e': {
          end_flag = create_sprite(flag, j * block_width, i * block_width - 28, 0, 0);
          break;
        }

        default: { // ignore new lines and other chars
          --j;
          break;
        }
      }
    }
  }

  fclose(ptr);
  draw_level();
  draw_sprite(character);

  old_purple = *create_sprite(purple_1_down, 0, 0, 0, 0); // allocate memory
  old_green = *create_sprite(green_1_down, 0, 0, 0, 0);

  cursor = create_sprite(cursor_xpm, 600, 500, 0, 0);
  old_cursor = *(cursor);
  draw_sprite(cursor);

  return 0;
}

int editor_init() {
  block_num = 68;
  blocks = (Sprite **) malloc(block_num * sizeof(Sprite *)); // maximum of blocks
  int index = 0;
  for (int i = 0; i < 20; ++i) { // draw the limits of the level with white blocks
    grid[0][i] = 1;
    grid[15][i] = 1;
    blocks[index++] = create_sprite(white_block, i * block_width, 0, 0, 0);
    blocks[index++] = create_sprite(white_block, i * block_width, 15 * block_width, 0, 0);
  }

  for (int i = 1; i < 15; ++i) {
    grid[i][0] = 1;
    grid[i][19] = 1;
    blocks[index++] = create_sprite(white_block, 0, i * block_width, 0, 0);
    blocks[index++] = create_sprite(white_block, 19 * block_width, i * block_width, 0, 0);
  }

  character = create_sprite(chell, 2 * block_width, 13 * block_width, 0, 0);
  grid[13][2] = 3;

  end_flag = create_sprite(flag, 18 * block_width, 14 * block_width - 28, 0, 0);
  grid[14][18] = 4;

  draw_sprite(character);
  flip_page();
  draw_sprite(character);
  flip_page();

  draw_level();

  cursor = create_sprite(cursor_xpm, 600, 500, 0, 0);
  old_cursor = *(cursor);
  draw_sprite(cursor);

  return 0;
}

int editor_end() {
  char next_path[] = "/Levels/next.txt";
  char levels_path[] = "/Levels/";

  char fileLevel[70];
  strncpy(fileLevel, resources_path, 50);
  strcat(fileLevel, next_path);

  char path[70];
  strncpy(path, resources_path, 50);
  strcat(path, levels_path);

  FILE *ptr;
  if ((ptr = fopen(fileLevel, "r")) == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  char fileLevel2[70];
  strncpy(fileLevel2, resources_path, 50);
  strcat(fileLevel2, next_path);

  int level = 0;
  fscanf(ptr, "%d", &level);
  char levelString[4];
  int temp = level + 1;
  sprintf(levelString, "%d", level);
  char suffix[] = ".txt";
  strcat(levelString, suffix);
  strcat(path, levelString);

  char nextLevel[4];
  sprintf(nextLevel, "%d", temp);

  fclose(ptr);

  if ((ptr = fopen(fileLevel2, "w")) == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  fprintf(ptr, "%s", nextLevel);
  fclose(ptr);

  FILE *ptr2;

  if ((ptr2 = fopen(path, "w")) == NULL) {
    printf("Error opening file\n");
    return 1;
  }

  fprintf(ptr2, "%d\n", block_num);

  for (int l = 0; l < 16; ++l) {
    for (int c = 0; c < 20; ++c) {
      char gridChar = grid[l][c];

      switch (gridChar) {
        case 0:
          fprintf(ptr2, " ");
          break;
        case 1:
          fprintf(ptr2, "w");
          break;
        case 2:
          fprintf(ptr2, "b");
          break;
        case 3:
          fprintf(ptr2, "s");
          break;
        case 4:
          fprintf(ptr2, "e");
          break;
        default:
          printf("Error writing file\n");
          return 1;
      }
    }
    fprintf(ptr2, "\n");
  }

  fclose(ptr2);

  destroy_all_sprites();
  game_state = MENU;
  return 0;
}

void destroy_all_sprites() {
  erase_everything();
  flip_page();
  erase_everything();
  flip_page();
  destroy_sprite(cursor);
  destroy_sprite(character);
  destroy_sprite(character2);
  destroy_sprite(end_flag);
  destroy_asprite(purple_portal);
  destroy_asprite(green_portal);
  purple_portal = NULL;
  green_portal = NULL;
  destroy_sprite(&old_green);
  destroy_sprite(&old_purple);
  free(reversed_img);
  free(reversed_img2);
  for (int i = 0; i < block_num; ++i) destroy_sprite(blocks[i]);
  free(blocks);
}

bool check_level_end() {
  if (game_state == MAIN_GAME || game_state == COOP) {
    if (character->x <= end_flag->x + end_flag->width / 2 && character->x + character->width >= end_flag->x && character->y + character->height >= end_flag->y && character->y < end_flag->y + end_flag->height) {

      if (game_state == MAIN_GAME)
        return true;
      else {
        if (character2->x <= end_flag->x + end_flag->width / 2 && character2->x + character2->width >= end_flag->x && character2->y + character2->height >= end_flag->y && character2->y < end_flag->y + end_flag->height) {
          return true;
        }
        return false;
      }
    }
    return false;
  }
  return false;
}

void check_block_collisions(int blockswp[], int *num, Sprite *charac) {
  int index = 0;
  for (int i = 0; i < block_num; ++i) { // save the blocks with portals to draw them later
    Sprite *b = blocks[i];
    if (block_has_portal(b) && (in_a_portal(character) || in_a_portal(character2))) {
      blockswp[index] = i;
      index++;
      continue;
    }

    // checking y collisions
    if ((charac->x > b->x && charac->x < (b->x + b->width)) ||
        ((charac->x + charac->width) > b->x && (charac->x + charac->width) < (b->x + b->width))) {

      if ((charac->y + charac->height + charac->yspeed) >= b->y && charac->y < b->y) { //  will it hit above the block?
        charac->y = b->y - charac->height - 1;
        charac->yspeed = 0;
        if (charac->xspeed != 4 && charac->xspeed != 0 && charac->xspeed != -4)
          charac->xspeed = 0;
      }

      else if ((charac->y + charac->yspeed) <= (b->y + b->height) &&
               (charac->y + charac->height) > (b->y + b->height)) { //  will it hit under the block?
        charac->y = b->y + b->height + 1;
        charac->yspeed = 0;
      }
    }

    // checking x collisions
    if ((charac->y < (b->y + b->height) && (charac->y + charac->height) > (b->y + b->height)) ||
        ((charac->y + charac->height) < (b->y + b->height) && (charac->y + charac->height) > b->y)) {

      if ((charac->x + charac->width + charac->xspeed) >= b->x &&
          charac->x < b->x) { // will it hit on the left side of the block?
        charac->x = b->x - charac->width - 1;
        charac->xspeed = 0;
      }

      else if (charac->x + charac->xspeed <= (b->x + b->width) &&
               (charac->x + charac->width) > (b->x + b->width)) { // will it hit on the right side of the block?
        charac->x = b->x + b->width + 1;
        charac->xspeed = 0;
      }
    }
  }
  (*num) = index;
}

bool block_has_portal(Sprite *b) {

  // it's only possible to teleport with both portals open
  if (purple_portal == NULL || green_portal == NULL)
    return false;

  if (purple_portal != NULL) {
    Sprite *s = purple_portal->sp;
    //check the 4 sides of the sprite

    if (s->x + 1 > b->x && s->x < b->x + b->width &&
        s->y + 1 > b->y && s->y < b->y + b->height) // top left
      return true;

    if (s->x + s->width - 1 >= b->x && s->x + s->width <= b->x + b->width &&
        s->y + 1 > b->y && s->y < b->y + b->height) // top right
      return true;

    if (s->x + 1 > b->x && s->x < b->x + b->width &&
        s->y + s->height - 1 >= b->y && s->y + s->height <= b->y + b->height) // bottom left
      return true;

    if (s->x + s->width - 1 >= b->x && s->x + s->width <= b->x + b->width &&
        s->y + s->height - 1 >= b->y && s->y + s->height <= b->y + b->height) // bottom right
      return true;
  }

  if (green_portal != NULL) {
    Sprite *s = green_portal->sp;
    //check the 4 sides of the sprite

    if (s->x + 1 > b->x && s->x < b->x + b->width &&
        s->y + 1 > b->y && s->y < b->y + b->height)
      return true;

    if (s->x + s->width - 1 >= b->x && s->x + s->width <= b->x + b->width &&
        s->y + 1 > b->y && s->y < b->y + b->height)
      return true;

    if (s->x + 1 > b->x && s->x < b->x + b->width &&
        s->y + s->height - 1 >= b->y && s->y + s->height <= b->y + b->height)
      return true;

    if (s->x + s->width - 1 >= b->x && s->x + s->width <= b->x + b->width &&
        s->y + s->height - 1 >= b->y && s->y + s->height <= b->y + b->height)
      return true;
  }

  return false;
}

bool in_a_portal(Sprite *charac) {
  // it's only possible to teleport with both portals open
  if (purple_portal == NULL || green_portal == NULL)
    return false;
  
  if (charac == NULL) return false; // character not defined

  Sprite *p = purple_portal->sp;
  Sprite *g = green_portal->sp;

  switch (purple_type) {
    case PURPLE_RIGHT: // same as left

    case PURPLE_LEFT: {
      if (charac->y >= p->y && charac->y + character->height <= p->y + p->height) {
        if ((charac->x <= p->x + p->width && charac->x >= p->x) || (charac->x + charac->width <= p->x + p->width && charac->x + charac->width >= p->x) || (charac->x + charac->width > p->x + p->width && charac->x < p->x))
          return true;
      }
      break;
    }

    case PURPLE_UP: // same as down

    case PURPLE_DOWN: {
      if (charac->x >= p->x && charac->x + charac->width <= p->x + p->width) {
        if ((charac->y + charac->height <= p->y + p->height && charac->y + charac->height >= p->y) || (charac->y <= p->y + p->height && charac->y >= p->y) || (charac->y + charac->height > p->y + p->height && charac->y < p->y))
          return true;
      }
      break;
    }

    default:
      break;
  }

  switch (green_type) {
    case GREEN_RIGHT: // same as left

    case GREEN_LEFT: {
      if (charac->y >= g->y && charac->y + character->height <= g->y + g->height) {
        if ((charac->x <= g->x + g->width && charac->x >= g->x) || (charac->x + charac->width <= g->x + g->width && charac->x + charac->width >= g->x) || (charac->x + charac->width > g->x + g->width && charac->x < g->x))
          return true;
      }
      break;
    }

    case GREEN_UP: // same as down

    case GREEN_DOWN: {
      if (charac->x >= g->x && charac->x + charac->width <= g->x + g->width) {
        if ((charac->y + charac->height <= g->y + g->height && charac->y + charac->height >= g->y) || (charac->y <= g->y + g->height && charac->y >= g->y) || (charac->y + charac->height > g->y + g->height && charac->y < g->y))
          return true;
      }
      break;
    }

    default:
      break;
  }
  return false;
}

void check_portals(Sprite *character) {
  if (purple_portal == NULL || green_portal == NULL)
    return;

  switch (purple_type) {

    case PURPLE_RIGHT: {
      if (character->x < purple_portal->sp->x + purple_portal->sp->width && character->x > purple_portal->sp->x && character->y >= purple_portal->sp->y && character->y + character->height <= purple_portal->sp->y + purple_portal->sp->height) {
        teleport_character(false, character);
        return;
      }
      break;
    }

    case PURPLE_LEFT: {
      if (character->x + character->width < purple_portal->sp->x + purple_portal->sp->width && character->x + character->width > purple_portal->sp->x && character->y >= purple_portal->sp->y && character->y + character->height <= purple_portal->sp->y + purple_portal->sp->height) {
        teleport_character(false, character);
        return;
      }
      break;
    }

    case PURPLE_UP: {
      if (character->y + character->height < purple_portal->sp->y + purple_portal->sp->height && character->y + character->height > purple_portal->sp->y && character->x >= purple_portal->sp->x && character->x + character->width <= purple_portal->sp->x + purple_portal->sp->width) {
        teleport_character(false, character);
        return;
      }
      break;
    }

    case PURPLE_DOWN: {
      if (character->y < purple_portal->sp->y + purple_portal->sp->height && character->y > purple_portal->sp->y && character->x >= purple_portal->sp->x && character->x + character->width <= purple_portal->sp->x + purple_portal->sp->width) {
        teleport_character(false, character);
        return;
      }
      break;
    }

    default:
      break;
  }

  switch (green_type) {

    case GREEN_RIGHT: {
      if (character->x < green_portal->sp->x + green_portal->sp->width && character->x > green_portal->sp->x && character->y >= green_portal->sp->y && character->y + character->height <= green_portal->sp->y + green_portal->sp->height)

        teleport_character(true, character);

      return;
    }

    case GREEN_LEFT: {
      if (character->x + character->width < green_portal->sp->x + green_portal->sp->width && character->x + character->width > green_portal->sp->x && character->y >= green_portal->sp->y && character->y + character->height <= green_portal->sp->y + green_portal->sp->height)

        teleport_character(true, character);

      return;
    }

    case GREEN_UP: {
      if (character->y + character->height < green_portal->sp->y + green_portal->sp->height && character->y + character->height > green_portal->sp->y && character->x >= green_portal->sp->x && character->x + character->width <= green_portal->sp->x + green_portal->sp->width)

        teleport_character(true, character);

      return;
    }

    case GREEN_DOWN: {
      if (character->y < green_portal->sp->y + green_portal->sp->height && character->y > green_portal->sp->y && character->x >= green_portal->sp->x && character->x + character->width <= green_portal->sp->x + green_portal->sp->width)

        teleport_character(true, character);

      return;
    }

    default:
      break;
  }
}

void teleport_character(bool purple, Sprite *character) {
  if (purple) {

    switch (purple_type) {
      case PURPLE_RIGHT: {
        character->xspeed = -(abs(character->xspeed) + abs(character->yspeed));
        if (character->xspeed == -4)
          character->xspeed = -3;
        character->yspeed = 0;
        character->x = purple_portal->sp->x + purple_portal->sp->width - character->width - 2;
        character->y = purple_portal->sp->y + 7; // more or less the middle of the portal
        break;
      }

      case PURPLE_LEFT: {
        character->xspeed = abs(character->xspeed) + abs(character->yspeed);
        if (character->xspeed == 4)
          character->xspeed = 3;
        character->yspeed = 0;
        character->x = purple_portal->sp->x + 2;
        character->y = purple_portal->sp->y + 7;
        break;
      }

      case PURPLE_UP: {
        character->yspeed = abs(character->xspeed) + abs(character->yspeed);
        character->xspeed = 0;
        character->x = purple_portal->sp->x + 32;
        character->y = purple_portal->sp->y + purple_portal->sp->height - character->height + 1;
        break;
      }

      case PURPLE_DOWN: {
        character->yspeed = -(abs(character->xspeed) + abs(character->yspeed));
        character->xspeed = 0;
        character->x = purple_portal->sp->x + 32;
        character->y = purple_portal->sp->y - 1;
        break;
      }

      default:
        break;
    }
  }

  else {

    switch (green_type) {

      case GREEN_RIGHT: {
        character->xspeed = -(abs(character->xspeed) + abs(character->yspeed));
        if (character->xspeed == -4)
          character->xspeed = -3;
        character->yspeed = 0;
        character->x = green_portal->sp->x + green_portal->sp->width - character->width - 2;
        character->y = green_portal->sp->y + 7;
        break;
      }

      case GREEN_LEFT: {
        character->xspeed = abs(character->xspeed) + abs(character->yspeed);
        if (character->xspeed == 4)
          character->xspeed = 3;
        character->yspeed = 0;
        character->x = green_portal->sp->x + 2;
        character->y = green_portal->sp->y + 7;
        break;
      }

      case GREEN_UP: {
        character->yspeed = abs(character->xspeed) + abs(character->yspeed);
        character->xspeed = 0;
        character->x = green_portal->sp->x + 32;
        character->y = green_portal->sp->y + green_portal->sp->height - character->height + 1;
        break;
      }

      case GREEN_DOWN: {
        character->yspeed = -(abs(character->xspeed) + abs(character->yspeed));
        character->xspeed = 0;
        character->x = green_portal->sp->x + 32;
        character->y = green_portal->sp->y - 1;
        break;
      }

      default:
        break;
    }
  }
}

void key_char_event(uint8_t *data, Sprite *character, char *char_state) {
  int walk_speed;
  walk_speed = 4;

  switch (*char_state) {
    case STILL_RIGHT: {
      if (*data == A_KEY_PRESSED) {
        *char_state = MOVING_LEFT;
        character->xspeed -= walk_speed;
        flip_sprite(character);
      }
      else if (*data == D_KEY_PRESSED) {
        *char_state = MOVING_RIGHT;
        character->xspeed += walk_speed;
      }
      break;
    }

    case STILL_LEFT: {
      if (*data == A_KEY_PRESSED) {
        *char_state = MOVING_LEFT;
        character->xspeed -= walk_speed;
      }
      else if (*data == D_KEY_PRESSED) {
        *char_state = MOVING_RIGHT;
        character->xspeed += walk_speed;
        flip_sprite(character);
      }
      break;
    }

    case MOVING_LEFT: {
      if (*data == A_KEY_RELEASED) {
        *char_state = STILL_LEFT;
        if (character->xspeed < -walk_speed)
          character->xspeed += walk_speed;
        else if (character->xspeed < 0)
          character->xspeed = 0;
      }
      else if (*data == D_KEY_PRESSED) {
        *char_state = LEFT_AND_THEN_RIGHT;
        if (character->xspeed < -walk_speed)
          character->xspeed += walk_speed;
        else if (character->xspeed < 0)
          character->xspeed = 0;
      }
      break;
    }

    case MOVING_RIGHT: {
      if (*data == D_KEY_RELEASED) {
        *char_state = STILL_RIGHT;
        if (character->xspeed > walk_speed)
          character->xspeed -= walk_speed;
        else if (character->xspeed > 0)
          character->xspeed = 0;
      }
      else if (*data == A_KEY_PRESSED) {
        *char_state = RIGHT_AND_THEN_LEFT;
        if (character->xspeed > walk_speed)
          character->xspeed -= walk_speed;
        else if (character->xspeed > 0)
          character->xspeed = 0;
      }
      break;
    }

    case LEFT_AND_THEN_RIGHT: {
      if (*data == D_KEY_RELEASED) {
        *char_state = MOVING_LEFT;
        character->xspeed -= walk_speed;
      }
      else if (*data == A_KEY_RELEASED) {
        *char_state = MOVING_RIGHT;
        character->xspeed += walk_speed;
        flip_sprite(character);
      }
      break;
    }

    case RIGHT_AND_THEN_LEFT: {
      if (*data == D_KEY_RELEASED) {
        *char_state = MOVING_LEFT;
        character->xspeed -= walk_speed;
        flip_sprite(character);
      }
      else if (*data == A_KEY_RELEASED) {
        *char_state = MOVING_RIGHT;
        character->xspeed += walk_speed;
      }
      break;
    }
  }

  // jumping can happen in either state

  if (character->yspeed == 0 && (*data == W_KEY_PRESSED || *data == SPACE_KEY_PRESSED)) {
    character->yspeed = -9;
  }
}

void key_event_handler(uint8_t *data) {

  if (game_state == COOP) {
    if (*data != KBC_INV_KEY) {

      uint8_t mess[2];
      mess[0] = P_KBC;
      mess[1] = *data;
      int tries = 0;
      while (uart_write_message(mess, 2) != 0) {
        ++tries;
        if (tries > 4) {
          printf("Error: KBC byte not sent\n");
          if (*data == ESC_KEY_PRESSED){
            game_end();
            game_state = MENU;
            game_init();
          }
          return;
        }
        tickdelay(micros_to_ticks(P_DELAY));
      }
    }
  }

  if (game_state == MAIN_GAME || game_state == COOP) {
    if (*data == KBC_INV_KEY)
      return; // Discard an invalid input

    if (*data == ESC_KEY_PRESSED) {
      game_end();
      game_state = MENU;
      game_init();
      return;
    }

    key_char_event(data, character, &char_state);
  }

  else if (game_state == LEVEL_EDITOR) {

    if (*data == ESC_KEY_PRESSED) {
      game_end();
      game_state = MENU;
      game_init();
    }

    if (*data == D_KEY_PRESSED)
      editor_draw_element(DELETE);

    else if (*data == E_KEY_PRESSED)
      editor_draw_element(FLAG);

    else if (*data == S_KEY_PRESSED)
      editor_draw_element(CHARACTER);
  }
}

void mouse_event_handler(mouse_event event, int16_t x_mov, int16_t y_mov) {

  switch (event) {

    case MOUSE_LB_PRESSED: {
      if (game_state == MAIN_GAME)
        shoot_portal(true, character, cursor->x, cursor->y);
      else if (game_state == COOP) {
        // send the information to the other PC
        uint16_t x_16 = cursor->x;
        uint16_t y_16 = cursor->y;
        uint8_t xl, xm, yl, ym;
        util_get_LSB(x_16, &xl);
        util_get_LSB(y_16, &yl);
        util_get_MSB(x_16, &xm);
        util_get_MSB(y_16, &ym);

        uint8_t data[5];
        data[0] = P_NEW_PORTAL;
        data[1] = xm;
        data[2] = xl;
        data[3] = ym;
        data[4] = yl;

        if (uart_write_message(data, 5)) {
          printf("Uart send error\n");
          return;
        }
        shoot_portal(true, character, cursor->x, cursor->y);
      }
      else if (game_state == LEVEL_EDITOR)
        editor_draw_element(WHITE_BLOCK);
      else if (game_state == MENU)
        menu_change();
      break;
    }

    case MOUSE_RB_PRESSED: {
      if (game_state == MAIN_GAME)
        shoot_portal(false, character, cursor->x, cursor->y);
      else if (game_state == LEVEL_EDITOR)
        editor_draw_element(BLACK_BLOCK);
      break;
    }

    case MOUSE_MOVEMENT: {
      cursor->x += x_mov;
      cursor->y -= y_mov;
      if (cursor->x < 0)
        cursor->x = 0;
      if (cursor->y < 0)
        cursor->y = 0;
      if (cursor->x >= (int) get_h_res())
        cursor->x = get_h_res() - 1;
      if (cursor->y >= (int) get_v_res())
        cursor->y = get_v_res() - 1;
      break;
    }

    default:
      break;
  }
}

void uart_event_handler(uint8_t data[]) {
  if (game_state != COOP)
    return;               // Ignore potencial garbage
  uint8_t type = data[0]; // identify the message

  if (type == P_KBC) {
    if (data[1] == ESC_KEY_PRESSED) {
      game_end();
      game_state = MENU;
      game_init();
      return;
    }
    key_char_event(&data[1], character2, &char_state2);
  }
  else if (type == P_NEW_PORTAL) {
    uint16_t x = (((uint16_t) data[1]) << 8) | ((uint16_t) data[2]);
    uint16_t y = (((uint16_t) data[3]) << 8) | ((uint16_t) data[4]);
    shoot_portal(false, character2, x, y);
  }
}

void menu_change() {
  char button = check_buttons(cursor->x, cursor->y);
  if (button == 0)
    return; // no changes

  if (button == 'e') {
    game_state = OVER;
    return;
  }

  game_end();

  switch (button) {
    case 'p':
      game_state = MAIN_GAME;
      break;
    case 'l':
      game_state = LEVEL_EDITOR;
      break;
    case 'c':
      game_state = COOP;
      break;
    default:
      break;
  }
  game_init();
}

void shoot_portal(bool purple, Sprite *character, int x_c, int y_c) {
  float ang = atan2(-y_c + character->y, x_c - character->x);
  int rad = 0;
  int x = 0, y = 0;
  int oldx, oldy;

  while (true) {
    oldx = x;
    oldy = y;
    rad++;
    x = character->x + floor(rad * cos(ang));
    y = character->y - floor(rad * sin(ang));

    if (grid[y / block_width][x / block_width] == 2)
      return; // hit a black block
    else if (grid[y / block_width][x / block_width] == 1)
      break; // hit a white block
    
    if (x < 0 || y < 0 || x > (int) get_h_res() || y > (int) get_v_res()) return; // off limits
  }

  int block_x = (x / block_width) * block_width;
  int block_y = (y / block_width) * block_width;

  if (oldx < block_x) { // right
                        // check edge cases
    if (grid[block_y / block_width + 1][block_x / block_width] != 1) {
      if (grid[block_y / block_width - 1][block_x / block_width] != 1)
        return;
      else {
        block_y = block_y - 64;
      }
    }

    if (grid[block_y / block_width + 1][block_x / block_width - 1] != 0) {
      if (grid[block_y / block_width - 1][block_x / block_width - 1] != 0)
        return;
      else {
        block_y = block_y - 64;
      }
    }

    if (purple) {
      purple_type = PURPLE_RIGHT;
      if (purple_portal != NULL)
        destroy_asprite(purple_portal);
      purple_portal = create_portal(purple_type, block_x - 20, block_y);
    }
    else {
      green_type = GREEN_RIGHT;
      if (green_portal != NULL)
        destroy_asprite(green_portal);
      green_portal = create_portal(green_type, block_x - 20, block_y);
    }
  }

  else if (oldx > block_x + (int) block_width - 1) { // left

    if (grid[block_y / block_width + 1][block_x / block_width] != 1) {
      if (grid[block_y / block_width - 1][block_x / block_width] != 1)
        return;
      else {
        block_y = block_y - 64;
      }
    }

    if (grid[block_y / block_width + 1][block_x / block_width + 1] != 0) {
      if (grid[block_y / block_width - 1][block_x / block_width + 1] != 0)
        return;
      else {
        block_y = block_y - 64;
      }
    }

    if (purple) {
      purple_type = PURPLE_LEFT;
      if (purple_portal != NULL)
        destroy_asprite(purple_portal);
      purple_portal = create_portal(purple_type, block_x + block_width - 20, block_y);
    }
    else {
      green_type = GREEN_LEFT;
      if (green_portal != NULL)
        destroy_asprite(green_portal);
      green_portal = create_portal(green_type, block_x + block_width - 20, block_y);
    }
  }

  else if (oldy < block_y) { // down

    if (grid[block_y / block_width][block_x / block_width + 1] != 1) {
      if (grid[block_y / block_width][block_x / block_width - 1] != 1)
        return;
      else {
        block_x = block_x - 64;
      }
    }

    if (grid[block_y / block_width - 1][block_x / block_width + 1] != 0) {
      if (grid[block_y / block_width - 1][block_x / block_width - 1] != 0)
        return;
      else {
        block_x = block_x - 64;
      }
    }

    if (purple) {
      purple_type = PURPLE_DOWN;
      if (purple_portal != NULL)
        destroy_asprite(purple_portal);
      purple_portal = create_portal(purple_type, block_x, block_y - 20);
    }
    else {
      green_type = GREEN_DOWN;
      if (green_portal != NULL)
        destroy_asprite(green_portal);
      green_portal = create_portal(green_type, block_x, block_y - 20);
    }
  }

  else { // up

    if (grid[block_y / block_width][block_x / block_width + 1] != 1) {
      if (grid[block_y / block_width][block_x / block_width - 1] != 1)
        return;
      else {
        block_x = block_x - 64;
      }
    }

    if (grid[block_y / block_width + 1][block_x / block_width + 1] != 0) {
      if (grid[block_y / block_width + 1][block_x / block_width - 1] != 0)
        return;
      else {
        block_x = block_x - 64;
      }
    }

    if (purple) {
      purple_type = PURPLE_UP;
      if (purple_portal != NULL)
        destroy_asprite(purple_portal);
      purple_portal = create_portal(purple_type, block_x, block_y + block_width - 20);
    }
    else {
      green_type = GREEN_UP;
      if (green_portal != NULL)
        destroy_asprite(green_portal);
      green_portal = create_portal(green_type, block_x, block_y + block_width - 20);
    }
  }
}

Sprite *find_block(int x, int y) {
  for (int i = 0; i < block_num; ++i) {
    Sprite *b = blocks[i];
    if (b->x == x && b->y == y)
      return b;
  }
  return NULL;
}

int editor_draw_element(Sprite_type type) {

  if (type == CHARACTER) {
    int line = cursor->y / block_width;
    int col = cursor->x / block_width;

    if (line + 1 > 15)
      return 1;
    if (grid[line][col] != 0 || (grid[line + 1][col] != 0 && grid[line + 1][col] != 3))
      return 1;

    grid[character->y / block_width][character->x / block_width] = 0;
    character->x = col * block_width;
    character->y = line * block_width;
    grid[line][col] = 3;
  }

  else if (type == FLAG) {
    int line = cursor->y / block_width;
    int col = cursor->x / block_width;

    if (line - 1 < 0)
      return 1;
    if (grid[line][col] != 0 || (grid[line - 1][col] != 0 && grid[line - 1][col] != 4))
      return 1;

    grid[(end_flag->y + 28) / block_width][end_flag->x / block_width] = 0;
    end_flag->x = col * block_width;
    end_flag->y = line * block_width - 28;
    grid[line][col] = 4;
  }

  else if (type == WHITE_BLOCK) {
    int line = cursor->y / block_width;
    int col = cursor->x / block_width;

    if (grid[line][col] != 0)
      return 1;
    if (line - 1 >= 0)
      if (grid[line - 1][col] == 3)
        return 1;
    if (line + 1 <= 15)
      if (grid[line + 1][col] == 4)
        return 1;

    grid[line][col] = 1;

    block_num++;
    blocks = (Sprite **) realloc(blocks, block_num * sizeof(Sprite *));
    blocks[block_num - 1] = create_sprite(white_block, col * block_width, line * block_width, 0, 0);
  }

  else if (type == BLACK_BLOCK) {
    int line = cursor->y / block_width;
    int col = cursor->x / block_width;

    if (grid[line][col] != 0)
      return 1;
    if (line - 1 >= 0)
      if (grid[line - 1][col] == 3)
        return 1;
    if (line + 1 <= 15)
      if (grid[line + 1][col] == 4)
        return 1;

    grid[line][col] = 2;

    block_num++;
    blocks = (Sprite **) realloc(blocks, block_num * sizeof(Sprite *));
    blocks[block_num - 1] = create_sprite(black_block, col * block_width, line * block_width, 0, 0);
  }

  else if (type == DELETE) {
    int line = cursor->y / block_width;
    int col = cursor->x / block_width;
    char c = grid[line][col];

    if (c != 1 && c != 2)
      return 1; // we can't delete the spawn or the finish line, only change them

    grid[line][col] = 0;
    Sprite *b = find_block(col * block_width, line * block_width);
    block_num--;
    Sprite **new_array = (Sprite **) malloc(block_num * sizeof(Sprite *));

    for (size_t i = 0, j = 0; (int) i <= block_num; ++i, ++j) {
      if (blocks[i] == b) {
        --j;
        continue;
      }
      new_array[j] = blocks[i];
    }
    free(blocks);
    blocks = new_array;
  }
  editor_update = true;
  return 0;
}

void draw_level() {
  for (int i = 0; i < block_num; ++i) { // draw permanently in both buffers
    draw_sprite(blocks[i]);
    flip_page();
    draw_sprite(blocks[i]);
    flip_page();
  }
  if (end_flag != NULL) {
    draw_sprite(end_flag);
    flip_page();
    draw_sprite(end_flag);
    flip_page();
  }
  load_background();
}

void draw_new_frame() {

  // we need to erase the previous frame after drawing a new one (because of page flipping!)

  Sprite *oldc, *oldc2;

  if (game_state == COOP) {
      if (char_reversed2) {
        oldc2 = create_sprite(chell_r, character2->x, character2->y, 0, 0);
        if (char_state2 == STILL_RIGHT || char_state2 == MOVING_RIGHT || char_state2 == RIGHT_AND_THEN_LEFT)
          char_reversed2 = false;
      }
      else {
        oldc2 = create_sprite(chell, character2->x, character2->y, 0, 0);
        if (char_state2 == STILL_LEFT || char_state2 == MOVING_LEFT || char_state2 == LEFT_AND_THEN_RIGHT)
          char_reversed2 = true;
      }

      if (frame_counter % 4 == 0)
        ++character2->yspeed; // gravity
      if (character2->yspeed == 0)
        ++character2->yspeed; // quick fix for double-jumping

      int blocks_with_portal2[4];
      int num2;
      check_block_collisions(blocks_with_portal2, &num2, character2);

      check_portals(character2);
      if (character2->yspeed > 40)
        character2->yspeed = 40; // so the players don't break the game in an infinit loop
      animate_sprite(character2);
    }

  if (game_state == MAIN_GAME || game_state == COOP) {

    if (char_reversed) {
      oldc = create_sprite(chell_r, character->x, character->y, 0, 0);
      if (char_state == STILL_RIGHT || char_state == MOVING_RIGHT || char_state == RIGHT_AND_THEN_LEFT)
        char_reversed = false;
    }
    else {
      oldc = create_sprite(chell, character->x, character->y, 0, 0);
      if (char_state == STILL_LEFT || char_state == MOVING_LEFT || char_state == LEFT_AND_THEN_RIGHT)
        char_reversed = true;
    }

    if (frame_counter % 4 == 0)
      ++character->yspeed; // gravity
    if (character->yspeed == 0)
      ++character->yspeed; // quick fix for double-jumping

    int blocks_with_portal[4];
    int num;
    check_block_collisions(blocks_with_portal, &num, character);

    check_portals(character);
    if (character->yspeed > 40)
      character->yspeed = 40; // so the players don't break the game in an infinit loop
    animate_sprite(character);

    // draw the blocks where the character may be
    for (int i = 0; i < num; ++i) draw_sprite(blocks[blocks_with_portal[i]]);

    if (purple_portal != NULL) {
      animate_asprite(purple_portal);
    }
    if (green_portal != NULL) {
      animate_asprite(green_portal);
    }

    if (check_level_end()) {
      //destroy_all_sprites();
      char_reversed = false;
      char_state = 0;
      if (game_state == COOP){
        char_reversed2 = false;
        char_state2 = 0;
      }
      read_level_from_file();
      return;
    }
  }

  if (game_state == LEVEL_EDITOR && editor_update) {
    erase_everything(); // maybe change this
    for (int i = 0; i < block_num; ++i)
      draw_sprite(blocks[i]);

    draw_sprite(end_flag);
    draw_sprite(character);
    load_background();
  }

  Sprite old_time_sprites[19];
  draw_time(old_time_sprites); // time always shows up
  draw_sprite(cursor);         // cursor is always used
  buffer_screen();

  // erase the previous frame

  flip_page();

  if (game_state == MAIN_GAME || game_state == COOP) {

    erase_sprite(oldc);
    destroy_sprite(oldc);

    if (game_state == COOP) {
      erase_sprite(oldc2);
      destroy_sprite(oldc2);
    }

    if (purple_portal != NULL)
      erase_sprite(&old_purple);
    if (green_portal != NULL)
      erase_sprite(&old_green);

    if (purple_portal != NULL) {
      old_purple.x = purple_portal->sp->x;
      old_purple.y = purple_portal->sp->y;
      old_purple.width = purple_portal->sp->width;
      old_purple.height = purple_portal->sp->height;
      memcpy(old_purple.map, purple_portal->sp->map, old_purple.width * old_purple.height * 2);
    }
    if (green_portal != NULL) {
      old_green.x = green_portal->sp->x;
      old_green.y = green_portal->sp->y;
      old_green.width = green_portal->sp->width;
      old_green.height = green_portal->sp->height;
      memcpy(old_green.map, green_portal->sp->map, old_green.width * old_green.height * 2);
    }
  }

  if (game_state == LEVEL_EDITOR && editor_update) {
    erase_everything();

    for (int i = 0; i < block_num; ++i)
      draw_sprite(blocks[i]);

    draw_sprite(end_flag);
    draw_sprite(character);
    editor_update = false;
  }

  erase_sprite(&old_cursor);
  old_cursor = (*cursor);

  for (int i = 0; i < 19; ++i)
    erase_sprite(&old_time[i]);
  memcpy(old_time, old_time_sprites, 19 * sizeof(Sprite));

  frame_counter++;
}

Game_state get_game_state() {
  return game_state;
}

void set_resources_path(char path[]){
  strncpy(resources_path, path, 50);
}
