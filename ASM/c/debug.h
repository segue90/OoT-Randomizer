#include "z64.h"
#include "gfx.h"
#include "text.h"

void draw_debug_int(int whichNumber, int numberToShow);
void draw_debug_float(int whichNumber, float numberToShow);
void draw_debug_numbers(z64_disp_buf_t *db);

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
