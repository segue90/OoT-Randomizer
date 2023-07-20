#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_MODE 0

#include "z64.h"
#include "gfx.h"
#include "text.h"
#include "item_effects.h"

typedef struct {
    uint8_t main_index;
    uint8_t sub_menu_index;
    uint8_t dungeon_index;
    uint8_t overworld_index;
    uint8_t item_index;
} menu_index_t;

typedef struct {
    uint8_t index;
    char name[15];
} menu_category_t;

typedef struct {
    uint8_t index;
    uint16_t entrance_index;
    char name[30];
} warp_t;

typedef struct {
    uint8_t index;
    uint16_t item_index;
    char name[30];
} item_t;

void debug_utilities(z64_disp_buf_t *db);
void draw_debug_menu(z64_disp_buf_t *db);
void draw_debug_numbers(z64_disp_buf_t *db);
int debug_menu_is_drawn();

#endif
