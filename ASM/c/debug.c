#include "debug.h"

const int debug_text_width = 10;
const int debug_text_height = 10;
colorRGB8_t debug_text_color = { 0xE0, 0xE0, 0x10 }; // Yellow

int show_input_viewer = 0;
int show_warp_menu = 0;

uint32_t debugNumbers[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float debugNumbersFloat[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

menu_index_t current_menu_indexes = {0, 0, 0, 0, 0};

menu_category_t menu_categories[] = {
    {  0, "Dungeons"},
    {  1, "Overworld"},
    {  2, "Switch Age"},
};

warp_t dungeon_warps[] = {
    {  0, 0x00, "Deku"},
    {  1, 0x04, "Dodongo"},
    {  2, 0x28, "Jabu"},
    {  3, 0x169, "Forest"},
    {  4, 0x165, "Fire"},
    {  5, 0x10, "Water"},
    {  6, 0x37, "Shadow"},
    {  7, 0x82, "Spirit"},
    {  8, 0x98, "BotW"},
    {  9, 0x88, "Ice"},
    { 10, 0x486, "Hideout"},
    { 11, 0x08, "GTG"},
    { 12, 0x467, "Ganon Castle"},
};


warp_t overworld_warps[] = {
    {  0, 0x443, "Kokiri Forest"},
    {  1, 0x11E, "Lost Woods"},
    {  2, 0xFC, "Sacred Forest Meadow"},
    {  3, 0x157, "Lon Lon Ranch"},
    {  4, 0xB1, "Market"},
    {  5, 0x138, "Hyrule Castle/OGC"},
    {  6, 0xDB, "Kakariko"},
    {  7, 0xE4, "Graveyard"},
    {  8, 0x13D, "DMT (from Kak)"},
    {  9, 0x1BD, "DMT (from Crater)"},
    { 10, 0x14D, "Goron City"},
    { 11, 0xEA, "Zora River (from Field)"},
    { 12, 0x108, "Zora Domain"},
    { 13, 0x225, "Zora Fountain"},
    { 14, 0x102, "Lake Hylia"},
    { 15, 0x117, "Gerudo Valley"},
    { 16, 0x129, "Gerudo Fortress"},
    { 17, 0x130, "Haunted Wasteland"},
    { 18, 0x123, "Desert Colossus"},
    { 19, 0x1FD, "Hyrule Field (from Market)"},
};

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

void debug_utilities(z64_disp_buf_t *db)
{
    if (!DEBUG_MODE){
        return;
    }

    // Press L to levitate
    // Shoutouts to glankk
    if (z64_game.common.input[0].raw.pad.l) {
        z64_link.common.vel_1.y = 6.34375f;
    }

    draw_debug_menu(db);
    draw_debug_numbers(db);
}

int debug_menu_is_drawn() {
    return show_warp_menu;
}

void draw_debug_menu(z64_disp_buf_t *db) {
    if (z64_game.common.input[0].pad_pressed.l && z64_game.common.input[0].pad_pressed.r) {
        show_warp_menu = show_warp_menu ? 0 : 1;
    }
    draw_debug_int(0, current_menu_indexes.overworld_index);
    if (show_warp_menu) {

        if (current_menu_indexes.sub_menu_index == 0) {

            if (z64_game.common.input[0].pad_pressed.dr) {
                current_menu_indexes.main_index++;
                if (current_menu_indexes.main_index > 2) {
                    current_menu_indexes.main_index = 0;
                }
            }
            if (z64_game.common.input[0].pad_pressed.dl) {
                if (current_menu_indexes.main_index == 0) {
                    current_menu_indexes.main_index = 2;
                }
                else {
                    current_menu_indexes.main_index--;
                }
            }

            if (current_menu_indexes.main_index == 2) {
                if (z64_game.common.input[0].pad_pressed.a) {
                    int age = z64_file.link_age;
                    z64_file.link_age = z64_game.link_age;
                    z64_game.link_age = !z64_game.link_age;
                    //z64_SwitchAgeEquips();
                    z64_file.link_age = age;
                    /*for (int i = 0; i < 4; ++i) {
                        if (z64_file.button_items[i] != Z64_ITEM_NULL) {
                            z64_UpdateItemButton(&z64_game, i);
                        }
                    }
                    z64_UpdateEquipment(&z64_game, &z64_link);*/
                    z64_file.entrance_index = z64_file.entrance_index;
                    z64_game.entrance_index = z64_file.entrance_index;
                    z64_game.scene_load_flag = 0x14;
                    z64_game.fadeout_transition = 0x02;
                    show_warp_menu = 0;
                }
            }
            else {
                if (z64_game.common.input[0].pad_pressed.a) {
                    current_menu_indexes.sub_menu_index++;
                }
            }
        }
        else { // in a sub menu

            if (z64_game.common.input[0].pad_pressed.b) {
                current_menu_indexes.sub_menu_index--;
            }

            switch (current_menu_indexes.main_index)
            {
                case 0: // Dungeons
                    if (z64_game.common.input[0].pad_pressed.dr) {
                        current_menu_indexes.dungeon_index++;
                        if (current_menu_indexes.dungeon_index > 12) {
                            current_menu_indexes.dungeon_index = 0;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.dl) {
                        if (current_menu_indexes.dungeon_index == 0) {
                            current_menu_indexes.dungeon_index = 12;
                        }
                        else {
                            current_menu_indexes.dungeon_index--;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.a) {
                        warp_t *d = &(dungeon_warps[current_menu_indexes.dungeon_index]);
                        z64_file.entrance_index = d->entrance_index;
                        z64_game.entrance_index = d->entrance_index;
                        z64_game.scene_load_flag = 0x14;
                        z64_game.fadeout_transition = 0x02;
                        show_warp_menu = 0;
                    }
                    break;
                case 1: // Overworld
                    if (z64_game.common.input[0].pad_pressed.dr) {
                        current_menu_indexes.overworld_index++;
                        if (current_menu_indexes.overworld_index > 19) {
                            current_menu_indexes.overworld_index = 0;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.dl) {
                        if (current_menu_indexes.overworld_index == 0) {
                            current_menu_indexes.overworld_index = 19;
                        }
                        else {
                            current_menu_indexes.overworld_index--;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.a) {
                        warp_t *d = &(overworld_warps[current_menu_indexes.overworld_index]);
                        z64_file.entrance_index = d->entrance_index;
                        z64_game.entrance_index = d->entrance_index;
                        z64_game.scene_load_flag = 0x14;
                        z64_game.fadeout_transition = 0x02;
                        show_warp_menu = 0;
                    }
                    break;
                default:
                    break;
            }
        }

        // Call setup display list
        gSPDisplayList(db->p++, &setup_db);

        // Set up dimensions

        int icon_size = 12;
        int font_width = 6;
        int font_height = 11;
        int padding = 1;
        int rows = 13;
        int bg_width =
            (6 * icon_size) +
            (11 * font_width) +
            (8 * padding);
        int bg_height = (rows * icon_size) + ((rows + 1) * padding);
        int bg_left = (Z64_SCREEN_WIDTH - bg_width) / 2;
        int bg_top = (Z64_SCREEN_HEIGHT - bg_height) / 2;

        int left = bg_left + padding;
        int start_top = bg_top + padding;

        // Draw background

        gDPSetCombineMode(db->p++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
        gDPSetPrimColor(db->p++, 0, 0, 0x00, 0x00, 0x00, 0xD0);
        gSPTextureRectangle(db->p++,
                bg_left<<2, bg_top<<2,
                (bg_left + bg_width)<<2, (bg_top + bg_height)<<2,
                0,
                0, 0,
                1<<10, 1<<10);

        gDPPipeSync(db->p++);
        gDPSetCombineMode(db->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

        if (current_menu_indexes.sub_menu_index == 0) {
            gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
            for (int i = 0; i < 5; i++) {
                menu_category_t *d = &(menu_categories[i]);
                int top = start_top + ((icon_size + padding) * i) + 1;
                if (i != current_menu_indexes.main_index) {
                    text_print_size(d->name, left, top, font_width);
                }
            }
            text_flush_size(db, font_width, font_height, 0, 0);

            gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
            menu_category_t *d = &(menu_categories[current_menu_indexes.main_index]);
            int top = start_top + ((icon_size + padding) * current_menu_indexes.main_index) + 1;
            text_print_size(d->name, left, top, font_width);
            text_flush_size(db, font_width, font_height, 0, 0);
        }
        else {
            switch (current_menu_indexes.main_index)
            {
                case 0: // Dungeons
                    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    for (int i = 0; i < 13; i++) {
                        warp_t *d = &(dungeon_warps[i]);
                        int top = start_top + ((icon_size + padding) * i) + 1;
                        if (i != current_menu_indexes.dungeon_index) {
                            text_print_size(d->name, left, top, font_width);
                        }
                    }
                    text_flush_size(db, font_width, font_height, 0, 0);

                    gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
                    warp_t *d = &(dungeon_warps[current_menu_indexes.dungeon_index]);
                    int top = start_top + ((icon_size + padding) * current_menu_indexes.dungeon_index) + 1;
                    text_print_size(d->name, left, top, font_width);
                    text_flush_size(db, font_width, font_height, 0, 0);
                    break;

                case 1: // Overworld
                    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    if (current_menu_indexes.overworld_index < 10) {
                        for (int i = 0; i < 10; i++) {
                            warp_t *d = &(overworld_warps[i]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i != current_menu_indexes.overworld_index) {
                                text_print_size(d->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10, 0xFF);
                        d = &(overworld_warps[current_menu_indexes.overworld_index]);
                        top = start_top + ((icon_size + padding) * current_menu_indexes.overworld_index) + 1;
                        text_print_size(d->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    else {
                        for (int i = 0; i < 10; i++) {
                            warp_t *d = &(overworld_warps[i + 10]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i + 10 != current_menu_indexes.overworld_index) {
                                text_print_size(d->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
                        d = &(overworld_warps[current_menu_indexes.overworld_index]);
                        top = start_top + ((icon_size + padding) * (current_menu_indexes.overworld_index - 10)) + 1;
                        text_print_size(d->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    break;
                default:
                    break;
            }
        }
    }
    gDPFullSync(db->p++);
    gSPEndDisplayList(db->p++);
}

void draw_debug_numbers(z64_disp_buf_t *db) {


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
    gDPSetPrimColor(db->p++, 0, 0, debug_text_color.r, debug_text_color.g, debug_text_color.b, 0xFF);
    text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    gDPFullSync(db->p++);
    gSPEndDisplayList(db->p++);
}

void draw_a(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.a) {
        //text_print_size("A", Z64_SCREEN_WIDTH / 12 + 8*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12 , debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
        gDPSetPrimColor(db->p++, 0, 0, 0x00, 0x00, 0xFF, 0xFF); // blue
        sprite_load(db, &button_sprite, 0, 5);
        sprite_draw(db, &button_sprite, 0, Z64_SCREEN_WIDTH / 12 + 8*font_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
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
