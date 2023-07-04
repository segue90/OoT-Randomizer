#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG_MODE 0

#include "z64.h"
#include "gfx.h"
#include "text.h"

typedef struct {
    uint8_t main_index;
    uint8_t sub_menu_index;
    uint8_t dungeon_index;
    uint8_t overworld_index;
    uint8_t interior_index;
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

void draw_debug_int(int whichNumber, int numberToShow);
void draw_debug_float(int whichNumber, float numberToShow);
void debug_utilities(z64_disp_buf_t *db);
void draw_debug_menu(z64_disp_buf_t *db);
void draw_debug_numbers(z64_disp_buf_t *db);
int debug_menu_is_drawn();

// Input viewer functions
void draw_a(z64_disp_buf_t *db);
void draw_b(z64_disp_buf_t *db);
void draw_a(z64_disp_buf_t *db);
void draw_start(z64_disp_buf_t *db);
void draw_cup(z64_disp_buf_t *db);
void draw_cdown(z64_disp_buf_t *db);
void draw_cleft(z64_disp_buf_t *db);
void draw_cright(z64_disp_buf_t *db);
void draw_z(z64_disp_buf_t *db);
void draw_l(z64_disp_buf_t *db);
void draw_r(z64_disp_buf_t *db);
void draw_x_stick(z64_disp_buf_t *db);
void draw_y_stick(z64_disp_buf_t *db);

#endif
