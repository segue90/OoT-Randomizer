#include "debug.h"

const int debug_text_width = 16;
const int debug_text_height = 16;
colorRGB8_t debug_text_color = { 0xE0, 0xE0, 0x10 }; // Yellow

int show_input_viewer = 0;
int show_warp_menu = 0;

uint32_t debugNumbers[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

menu_index_t current_menu_indexes = {0, 0, 0, 0, 0};

menu_category_t menu_categories[] = {
    {  0, "Dungeons"},
    {  1, "Overworld"},
    {  2, "Items"},
    {  3, "Switch Age"},
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
    //{ 19, 0x1FD, "Hyrule Field (from Market)"},
};

item_t items_debug[] = {
    {  0, Z64_ITEM_HOOKSHOT, "Hookshot"},
    {  1, Z64_ITEM_LONGSHOT, "Longshot"},
    {  2, Z64_ITEM_BOW, "Bow"},
    {  3, Z64_ITEM_BOMB_BAG_20,  "Bomb bag"},
    {  4, Z64_ITEM_DEKU_SHIELD, "Deku Shield"},
    {  5, Z64_ITEM_HYLIAN_SHIELD,  "Hylian Shield"},
    {  6, Z64_ITEM_MIRROR_SHIELD, "Mirror Shield"},
    {  7, Z64_ITEM_FAIRY_OCARINA, "Ocarina"},
    {  8, Z64_ITEM_BOMBCHU, "Bombchu"},
    {  9, Z64_ITEM_BOOMERANG, "Boomerang"},
    {  10, Z64_ITEM_HAMMER, "Hammer"},
    {  11, Z64_ITEM_BOTTLE, "Bottle"},
    {  12, Z64_ITEM_KOKIRI_SWORD, "Kokiri Sword"},
    {  13, Z64_ITEM_BIGGORON_SWORD, "Biggoron Sword"},
    {  14, Z64_ITEM_IRON_BOOTS, "Iron Boots"},
    {  15, Z64_ITEM_HOVER_BOOTS, "Hover Boots"},
    {  16, Z64_ITEM_GORONS_BRACELET, "Goron Bracelet"},
    {  17, Z64_ITEM_SILVER_GAUNTLETS, "Silver Gauntlets"},
    {  18, Z64_ITEM_GOLDEN_GAUNTLETS, "Golden Gauntlets"},
    {  19, Z64_ITEM_SILVER_SCALE, "Scale"},
    {  20, Z64_ITEM_MINUET, "Minuet of Forest"},
    {  21, Z64_ITEM_BOLERO, "Bolero of Fire"},
    {  22, Z64_ITEM_SERENADE, "Serenade of Water"},
    {  23, Z64_ITEM_REQUIEM, "Requiem of Spirit"},
    {  24, Z64_ITEM_NOCTURNE, "Nocturne of Shadow"},
    {  25, Z64_ITEM_PRELUDE, "Prelude of Light"},
    {  26, Z64_ITEM_ZELDAS_LULLABY, "Zeldas Lullaby"},
    {  27, Z64_ITEM_EPONAS_SONG, "Eponas Song"},
    {  28, Z64_ITEM_SARIAS_SONG, "Sarias Song"},
    {  29, Z64_ITEM_SUNS_SONG, "Suns Song"},
    {  30, Z64_ITEM_SONG_OF_TIME, "Song of Time"},
    {  31, Z64_ITEM_SONG_OF_STORMS, "Song of Storms"},
    {  32, Z64_ITEM_FOREST_MEDALLION, "Forest Medallion"},
    {  33, Z64_ITEM_FIRE_MEDALLION, "Fire Medallion"},
    {  34, Z64_ITEM_WATER_MEDALLION, "Water Medallion"},
    {  35, Z64_ITEM_SPIRIT_MEDALLION, "Spirit Medallion"},
    {  36, Z64_ITEM_SHADOW_MEDALLION, "Shadow Medallion"},
    {  37, Z64_ITEM_LIGHT_MEDALLION, "Light Medallion"},
    {  38, Z64_ITEM_KOKIRIS_EMERALD, "Kokiri Emerald"},
    {  39, Z64_ITEM_GORONS_RUBY, "Goron Ruby"},
    {  40, Z64_ITEM_ZORAS_SAPPHIRE, "Zora Sapphire"},
};

void draw_debug_int(int whichNumber, int numberToShow) {
    if (whichNumber < 0 || whichNumber > 9) {
        return;
    }
    debugNumbers[whichNumber] = numberToShow;
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

    if (show_warp_menu) {

        if (current_menu_indexes.sub_menu_index == 0) {

            if (z64_game.common.input[0].pad_pressed.dr) {
                current_menu_indexes.main_index++;
                if (current_menu_indexes.main_index > 3) {
                    current_menu_indexes.main_index = 0;
                }
            }
            if (z64_game.common.input[0].pad_pressed.dl) {
                if (current_menu_indexes.main_index == 0) {
                    current_menu_indexes.main_index = 3;
                }
                else {
                    current_menu_indexes.main_index--;
                }
            }

            if (current_menu_indexes.main_index == 3) {
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
                case 2: // Items
                    if (z64_game.common.input[0].pad_pressed.dr) {
                        current_menu_indexes.item_index++;
                        if (current_menu_indexes.item_index > 40) {
                            current_menu_indexes.item_index = 0;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.dl) {
                        if (current_menu_indexes.item_index == 0) {
                            current_menu_indexes.item_index = 40;
                        }
                        else {
                            current_menu_indexes.item_index--;
                        }
                    }
                    if (z64_game.common.input[0].pad_pressed.a) {
                        item_t *d = &(items_debug[current_menu_indexes.item_index]);
                        // Songs don't work somehow? So use the rando function instead.
                        if ((d->item_index >= ITEM_SONG_MINUET) && (d->item_index <= ITEM_SONG_STORMS)) {
                            give_song(&z64_file, d->item_index - 0x54, 0);
                        }
                        else
                            z64_GiveItem(&z64_game, d->item_index);
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
            (13 * font_width) +
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
            for (int i = 0; i < 4; i++) {
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
                case 2: // Items
                    gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
                    if (current_menu_indexes.item_index < 10) {
                        for (int i = 0; i < 10; i++) {
                            item_t *dd = &(items_debug[i]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i != current_menu_indexes.item_index) {
                                text_print_size(dd->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10, 0xFF);
                        item_t *dd = &(items_debug[current_menu_indexes.item_index]);
                        top = start_top + ((icon_size + padding) * current_menu_indexes.item_index) + 1;
                        text_print_size(dd->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    if (current_menu_indexes.item_index > 9 && current_menu_indexes.item_index < 20) {
                        for (int i = 0; i < 10; i++) {
                            item_t *dd = &(items_debug[i + 10]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i + 10 != current_menu_indexes.item_index) {
                                text_print_size(dd->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
                        item_t *dd = &(items_debug[current_menu_indexes.item_index]);
                        top = start_top + ((icon_size + padding) * (current_menu_indexes.item_index - 10)) + 1;
                        text_print_size(dd->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    if (current_menu_indexes.item_index > 19 && current_menu_indexes.item_index < 32) {
                        for (int i = 0; i < 10; i++) {
                            item_t *dd = &(items_debug[i + 20]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i + 20 != current_menu_indexes.item_index) {
                                text_print_size(dd->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
                        item_t *dd = &(items_debug[current_menu_indexes.item_index]);
                        top = start_top + ((icon_size + padding) * (current_menu_indexes.item_index - 20)) + 1;
                        text_print_size(dd->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    if (current_menu_indexes.item_index > 31 && current_menu_indexes.item_index < 41) {
                        for (int i = 0; i < 9; i++) {
                            item_t *dd = &(items_debug[i + 32]);
                            int top = start_top + ((icon_size + padding) * i) + 1;
                            if (i + 32 != current_menu_indexes.item_index) {
                                text_print_size(dd->name, left, top, font_width);
                            }
                        }
                        text_flush_size(db, font_width, font_height, 0, 0);

                        gDPSetPrimColor(db->p++, 0, 0, 0xE0, 0xE0, 0x10 , 0xFF);
                        item_t *dd = &(items_debug[current_menu_indexes.item_index]);
                        top = start_top + ((icon_size + padding) * (current_menu_indexes.item_index - 32)) + 1;
                        text_print_size(dd->name, left, top, font_width);
                        text_flush_size(db, font_width, font_height, 0, 0);
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void draw_a(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.a) {
        //text_print_size("A", Z64_SCREEN_WIDTH / 12 + 8*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12 , debug_text_width);
        gDPSetPrimColor(db->p++, 0, 0, 0x00, 0x00, 0xFF, 0xFF); // blue
        sprite_draw(db, &buttons_sprite, 0, Z64_SCREEN_WIDTH / 12 + 3*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_b(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.b) {
       // text_print_size("B", Z64_SCREEN_WIDTH / 12 + 9*font_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        gDPSetPrimColor(db->p++, 0, 0, 0xFF, 0x00, 0x00, 0xFF); // blue
        sprite_draw(db, &buttons_sprite, 1, Z64_SCREEN_WIDTH / 12 + 4*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_start(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.s) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // grey
        text_print_size("S", Z64_SCREEN_WIDTH / 12 + 10*buttons_sprite.tile_w, 11 * Z64_SCREEN_HEIGHT / 12, debug_text_width);
        //text_flush_size(db, debug_text_width, debug_text_height, 0, 0);
    }
}

void draw_cdown(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cd) {
        gDPSetPrimColor(db->p++, 0, 0, 0xF4, 0xEC, 0x30, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 7, Z64_SCREEN_WIDTH / 12 + 5*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_cup(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cu) {
        gDPSetPrimColor(db->p++, 0, 0, 0xF4, 0xEC, 0x30, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 6, Z64_SCREEN_WIDTH / 12 + 8*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}


void draw_cleft(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cl) {
        gDPSetPrimColor(db->p++, 0, 0, 0xF4, 0xEC, 0x30, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 8, Z64_SCREEN_WIDTH / 12 + 7*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_cright(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.cr) {
        gDPSetPrimColor(db->p++, 0, 0, 0xF4, 0xEC, 0x30, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 9, Z64_SCREEN_WIDTH / 12 + 6*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_z(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.z) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // grey
        sprite_draw(db, &buttons_sprite, 5, Z64_SCREEN_WIDTH / 12 + 9*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_l(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.l) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // grey
        sprite_draw(db, &buttons_sprite, 3, Z64_SCREEN_WIDTH / 12 + 10*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_r(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.r) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // grey
        sprite_draw(db, &buttons_sprite, 4, Z64_SCREEN_WIDTH / 12 + 11*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_ddown(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.dd) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 7, Z64_SCREEN_WIDTH / 12 + 12*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_dup(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.du) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 6, Z64_SCREEN_WIDTH / 12 + 15*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}


void draw_dleft(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.dl) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 8, Z64_SCREEN_WIDTH / 12 + 14*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

void draw_dright(z64_disp_buf_t *db) {

    if (z64_game.common.input[0].raw.pad.dr) {
        gDPSetPrimColor(db->p++, 0, 0, 0xDC, 0xDC, 0xDC, 0xFF); // yellow
        sprite_draw(db, &buttons_sprite, 9, Z64_SCREEN_WIDTH / 12 + 13*buttons_sprite.tile_w,
         11 * Z64_SCREEN_HEIGHT / 12, debug_text_width, debug_text_height);
    }
}

// Helper function for drawing numbers to the HUD.
void draw_int_helper(z64_disp_buf_t *db, int32_t number, int16_t left, int16_t top, colorRGBA8_t color) {

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
    }
    while ( number > 0 );
    // This combiner mode makes it look like the rupee count
    gDPSetCombineLERP(db->p++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                            TEXEL0, 0, PRIMITIVE, 0);

    // Set the color
    gDPSetPrimColor(db->p++, 0, 0, color.r, color.g, color.b, color.a);
    if (isNegative) {
        text_print_size("-", left - rupee_digit_sprite.tile_w, top, 8);
        text_flush_size(db, 8, 16, 0, 0);
    }
    // Draw each digit
    for(uint8_t c = j; c > 0; c--) {
        sprite_texture(db, &rupee_digit_sprite, digits[c-1], left, top, 8, 16);
        left += 8;
    }
}

void draw_x_stick(z64_disp_buf_t *db) {
    colorRGBA8_t color = { 0xF4, 0xEC, 0x30, 0xFF};
    draw_int_helper(db, z64_game.common.input[0].raw.x, Z64_SCREEN_WIDTH / 12,
        11 * Z64_SCREEN_HEIGHT / 12, color);
}

void draw_y_stick(z64_disp_buf_t *db) {
    colorRGBA8_t color = { 0xF4, 0xEC, 0x30, 0xFF};
    draw_int_helper(db, z64_game.common.input[0].raw.y, Z64_SCREEN_WIDTH / 12 + 4*rupee_digit_sprite.tile_w,
        11 * Z64_SCREEN_HEIGHT / 12, color);
}


void draw_debug_numbers(z64_disp_buf_t *db) {

    for (int i = 0; i < 10; i++) {

        int numberToShow = debugNumbers[i];
        if (!numberToShow) {
            continue;
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

        colorRGBA8_t color = { 0xF4, 0xEC, 0x30, 0xFF};
        draw_int_helper(db, numberToShow, debug_text_x_placement, height + offsetY, color);
    }

    // Input viewer
    if (show_input_viewer) {
        sprite_load(db, &buttons_sprite, 0, 10);
        draw_a(db);
        draw_b(db);
        draw_cup(db);
        draw_cdown(db);
        draw_cleft(db);
        draw_cright(db);
        //draw_start(db);

        draw_z(db);
        draw_l(db);
        draw_r(db);

        draw_dup(db);
        draw_ddown(db);
        draw_dleft(db);
        draw_dright(db);

        draw_x_stick(db);
        draw_y_stick(db);
    }

}
