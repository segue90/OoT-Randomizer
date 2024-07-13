#include "text.h"

#include "gfx.h"
#include "util.h"
#include "z64.h"

#define text_max_chars 256
#define text_bucket_count 6
#define text_bucket_size 18

#define FONT_CHAR_TEX_WIDTH 16
#define FONT_CHAR_TEX_HEIGHT 16
#define NUM_FONT_CHARS 95

typedef struct {
    uint32_t c : 8;
    uint32_t left : 12;
    uint32_t top : 12;
} text_char_t;

uint8_t font_textures[NUM_FONT_CHARS * FONT_CHAR_TEX_WIDTH * FONT_CHAR_TEX_HEIGHT / 2] __attribute__ ((aligned (8)));

void print_char(z64_disp_buf_t* db, char c, int x, int y, int width, int height) {
    void* textureImage = &(font_textures[0]) + (c - ' ') * 16 * 16 / 2;
    int32_t sCharTexSize = ((width + height) / 16) * 16.0f;
    int32_t sCharTexScale = 1024.0f / ((width + height) / 16);

    gDPPipeSync(db->p++);
    gDPLoadTextureBlock_4b(db->p++, textureImage, G_IM_FMT_I, FONT_CHAR_TEX_WIDTH, FONT_CHAR_TEX_HEIGHT, 0,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gDPPipeSync(db->p++);
    // gSPTextureRectangle(db->p++, x * 4, y * 4, (x + width) * 4, (y * height) * 4, G_TX_RENDERTILE, 0,0, width_factor, height_factor);
    gSPTextureRectangle(db->p++, x << 2, y << 2, (x + sCharTexSize) << 2, (y + sCharTexSize) << 2, G_TX_RENDERTILE, 0, 0,
                        sCharTexScale, sCharTexScale);
}

void text_init() {
    DmaMgr_RequestSync(&(font_textures[0]), 0x928000, NUM_FONT_CHARS * FONT_CHAR_TEX_WIDTH * FONT_CHAR_TEX_HEIGHT / 2);
}

int text_print_size(z64_disp_buf_t* db, const char* s, int left, int top, int width, int height) {
    while(*s != 0x00) {
        print_char(db, *s, left, top, width, height);
        left += 8;
        s++;
    }

    return left;
}

int text_print(z64_disp_buf_t* db, const char* s, int left, int top) {
    return text_print_size(db, s, left, top, font_sprite.tile_w, font_sprite.tile_h);
}

void text_flush_size(z64_disp_buf_t* db, int width, int height, int hoffset, int voffset) {
    return;
}

void text_flush(z64_disp_buf_t* db) {
    text_flush_size(db, font_sprite.tile_w, font_sprite.tile_h, 0, 0);
}

int draw_int(z64_disp_buf_t* db, int32_t number, int16_t left, int16_t top, colorRGBA8_t color) {
    draw_int_size(db, number, left, top, color, 8, 16);
}

// Helper function for drawing numbers to the HUD.
int draw_int_size(z64_disp_buf_t* db, int32_t number, int16_t left, int16_t top, colorRGBA8_t color, int16_t width, int16_t height) {
    int isNegative = 0;
    if (number < 0) {
        number *= -1;
        isNegative = 1;
    }

    uint8_t digits[10];
    uint8_t j = 0;
    // Extract each digit. They are added, in reverse order, to digits[]
    do {
        digits[j] = number % 10;
        number = number / 10;
        j++;
    } while (number > 0);
    // This combiner mode makes it look like the rupee count
    gDPSetCombineLERP(db->p++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
        TEXEL0, 0, PRIMITIVE, 0);

    // Set the color
    gDPSetPrimColor(db->p++, 0, 0, color.r, color.g, color.b, color.a);
    if (isNegative) {
        text_print_size(db, "-", left - rupee_digit_sprite.tile_w, top, width, height);
        text_flush_size(db, width, height, 0, 0);
    }
    // Draw each digit
    for (uint8_t c = j; c > 0; c--) {
        sprite_texture(db, &rupee_digit_sprite, digits[c-1], left, top, width, height);
        left += width;
    }
    return j;
}
