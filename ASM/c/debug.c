#include "debug.h"

const int debug_text_width = 10;
const int debug_text_height = 10;
colorRGB8_t debug_text_color = { 0xE0, 0xE0, 0x10 }; // Yellow

int show_input_viewer = 0;

uint32_t debugNumbers[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float debugNumbersFloat[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

void draw_debug_int(int whichNumber, int numberToShow) {
    if (whichNumber < 0 || whichNumber > 9) {
        return;
    }
    debugNumbers[whichNumber] = numberToShow;
}

void draw_debug_float(int whichNumber, float numberToShow) {
    if (whichNumber < 0 || whichNumber > 9) {
        return;
    }
    debugNumbersFloat[whichNumber] = numberToShow;
}

void draw_debug_numbers(z64_disp_buf_t *db) {

    // Press L to levitate
    // Shoutouts to glankk
    if (z64_game.common.input[0].raw.pad.l) {
        z64_link.common.vel_1.y = 6.34375f;
    }

    gSPDisplayList(db->p++, &setup_db);
    gDPPipeSync(db->p++);
    gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(db->p++, 0, 0, debug_text_color.r, debug_text_color.g, debug_text_color.b, 0xFF);

    for (int i = 0; i < 10; i++) {

        int numberToShow = debugNumbers[i];
        if (!numberToShow) {
            continue;
        }

        int isNegative = 0;
        if (numberToShow < 0) {
            numberToShow *= -1;
            isNegative = 1;
        }

        int numberToShow_digits = 0;
        int numberToShow_copy = numberToShow;
        while (numberToShow_copy >= 1) {
            numberToShow_digits++;
            numberToShow_copy /= 10;
        }
        int str_len = numberToShow_digits;
        char text[str_len + 1 + isNegative];
        text[str_len + isNegative] = 0;
        numberToShow_copy = numberToShow;
        for (int j = numberToShow_digits - 1 + isNegative; j >= isNegative; j--) {
            text[j] = (numberToShow_copy % 10) + '0';
            numberToShow_copy /= 10;
        }
        if (isNegative) {
            text[0] = '-';
        }

        int debug_text_x_placement = Z64_SCREEN_WIDTH / 12;
        int debug_text_y_placement = font_sprite.tile_h * 2.5;

        int offsetY = i * font_sprite.tile_h;
        int height = debug_text_y_placement;
        // Move down if magic or 2nd row of hearts
        if (z64_file.magic_capacity_set > 0)
            height += font_sprite.tile_h * 0.8;
        if (z64_file.energy_capacity > 10 * 0x10)
            height += font_sprite.tile_h * 0.8;

        text_print_size(text, debug_text_x_placement, height + offsetY, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }

    for (int i = 0; i < 10; i++) {

        float numberToShow = debugNumbersFloat[i];
        if (!numberToShow) {
            continue;
        }

        int isNegative = 0;
        if (numberToShow < 0) {
            numberToShow *= -1;
            isNegative = 1;
        }

        // 4 decimals
        int floorValue = (int)numberToShow;
        int decimalValue = (numberToShow - floorValue) * 10000;

        int numberToShow_digits = 0;
        int numberToShow_copy = floorValue;
        while (numberToShow_copy >= 1) {
            numberToShow_digits++;
            numberToShow_copy /= 10;
        }
        int str_len = numberToShow_digits + isNegative + 5;
        char text[str_len + 1];
        text[str_len] = 0;
        numberToShow_copy = floorValue;
        for (int j = numberToShow_digits - 1 + isNegative; j >= isNegative; j--) {
            text[j] = (numberToShow_copy % 10) + '0';
            numberToShow_copy /= 10;
        }
        if (isNegative) {
            text[0] = '-';
        }

        int decimal_copy = decimalValue;
        decimal_copy = decimalValue;
        text[str_len - 5] = '.';
        for (int j = str_len - 1; j > str_len - 5; j--) {
            text[j] = (decimal_copy % 10) + '0';
            decimal_copy /= 10;
        }

        int debug_text_x_placement = Z64_SCREEN_WIDTH / 12;
        int debug_text_y_placement = font_sprite.tile_h * 2.5;

        int offsetY = i * font_sprite.tile_h;
        int height = debug_text_y_placement;
        // Move down if magic or 2nd row of hearts
        if (z64_file.magic_capacity_set > 0)
            height += font_sprite.tile_h * 0.8;
        if (z64_file.energy_capacity > 10 * 0x10)
            height += font_sprite.tile_h * 0.8;

        text_print_size(text, debug_text_x_placement, height + offsetY, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }

    // Input viewer
    if (show_input_viewer) {
        draw_a(db);
        draw_b(db);
        draw_x_stick(db);
        draw_y_stick(db);
        draw_start(db);
        draw_cup(db);
        draw_cdown(db);
        draw_cleft(db);
        draw_cright(db);
        draw_z(db);
        draw_l(db);
        draw_r(db);
    }

    text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    gDPFullSync(db->p++);
    gSPEndDisplayList(db->p++);
}

void draw_a(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.a) {
        text_print_size("A", Z64_SCREEN_WIDTH / 12 + 8*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12 , debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_b(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.b) {
        text_print_size("B", Z64_SCREEN_WIDTH / 12 + 9*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_start(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.s) {
        text_print_size("S", Z64_SCREEN_WIDTH / 12 + 10*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_cup(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cu) {
        text_print_size("^", Z64_SCREEN_WIDTH / 12 + 11*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_cdown(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cd) {
        text_print_size("v", Z64_SCREEN_WIDTH / 12 + 12*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_cleft(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cl) {
        text_print_size("<", Z64_SCREEN_WIDTH / 12 + 13*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_cright(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cr) {
        text_print_size(">", Z64_SCREEN_WIDTH / 12 + 14*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_z(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.z) {
        text_print_size("Z", Z64_SCREEN_WIDTH / 12 + 15*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_l(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.l) {
        text_print_size("L", Z64_SCREEN_WIDTH / 12 + 16*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_r(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.r) {
        text_print_size("R", Z64_SCREEN_WIDTH / 12 + 17*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_x_stick(z64_disp_buf_t *db) {

    int numberToShow = z64_game.common.input[0].raw.x;
    int isNegative = 0;
    if (numberToShow < 0) {
        numberToShow *= -1;
        isNegative = 1;
    }
    int numberToShow_digits = 0;
    int numberToShow_copy = numberToShow;
    while (numberToShow_copy >= 1) {
        numberToShow_digits++;
        numberToShow_copy /= 10;
    }
    int str_len = numberToShow_digits;
    char text[str_len + 1 + isNegative];
    text[str_len + isNegative] = 0;
    numberToShow_copy = numberToShow;
    for (int j = numberToShow_digits - 1 + isNegative; j >= isNegative; j--) {
        text[j] = (numberToShow_copy % 10) + '0';
        numberToShow_copy /= 10;
    }
    if (isNegative) {
        text[0] = '-';
    }

    text_print_size(text, Z64_SCREEN_WIDTH / 12 + (1 - isNegative)*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
    //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
}

void draw_y_stick(z64_disp_buf_t *db) {

    int numberToShow = z64_game.common.input[0].raw.y;
    int isNegative = 0;
    if (numberToShow < 0) {
        numberToShow *= -1;
        isNegative = 1;
    }
    int numberToShow_digits = 0;
    int numberToShow_copy = numberToShow;
    while (numberToShow_copy >= 1) {
        numberToShow_digits++;
        numberToShow_copy /= 10;
    }
    int str_len = numberToShow_digits;
    char text[str_len + 1 + isNegative];
    text[str_len + isNegative] = 0;
    numberToShow_copy = numberToShow;
    for (int j = numberToShow_digits - 1 + isNegative; j >= isNegative; j--) {
        text[j] = (numberToShow_copy % 10) + '0';
        numberToShow_copy /= 10;
    }
    if (isNegative) {
        text[0] = '-';
    }

    text_print_size(text, Z64_SCREEN_WIDTH / 12 + (5 - isNegative)*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
    //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
}
