#include "audio.h"
#include "gfx.h"
#include "kaleido_item.h"

void KaleidoScope_DrawItemSelect(z64_game_t* play) {
    static int16_t magicArrowEffectsR[] = { 255, 100, 255 };
    static int16_t magicArrowEffectsG[] = { 0, 100, 255 };
    static int16_t magicArrowEffectsB[] = { 0, 255, 100 };
    z64_input_t* input = &play->common.input[0];
    z64_pause_ctxt_t* pauseCtx = &play->pause_ctxt;
    uint16_t i;
    uint16_t j;
    uint16_t cursorItem;
    uint16_t cursorSlot;
    uint16_t index;
    int16_t cursorPoint;
    int16_t cursorX;
    int16_t cursorY;
    int16_t oldCursorPoint;
    int16_t moveCursorResult;

    OPEN_DISPS(play->common.gfx);

    gfx_setup_dl_42_opa(play->common.gfx);

    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    pauseCtx->cursor_color_set = 0;
    pauseCtx->name_color_set = 0;

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->changing == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->screen_idx == PAUSE_ITEM)) {
        oldCursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];
        cursorItem = pauseCtx->cursor_item[PAUSE_ITEM];
        cursorSlot = pauseCtx->cursor_slot[PAUSE_ITEM];
        moveCursorResult = 0;

        if (pauseCtx->cursor_special_pos == 0) {
            pauseCtx->cursor_color_set = 4;

            if (cursorItem == PAUSE_ITEM_NONE) {
                pauseCtx->stick_movement_x = 40;
            }

            if (ABS(pauseCtx->stick_movement_x) > 30) {
                cursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];
                cursorX = pauseCtx->cursor_x[PAUSE_ITEM];
                cursorY = pauseCtx->cursor_y[PAUSE_ITEM];

                while (moveCursorResult == 0) {
                    if (pauseCtx->stick_movement_x < -30) {
                        // Move left
                        if (pauseCtx->cursor_x[PAUSE_ITEM] != 0) {
                            pauseCtx->cursor_x[PAUSE_ITEM]--;
                            pauseCtx->cursor_point[PAUSE_ITEM] -= 1;

                            if(z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] != ITEM_NONE) {
                                moveCursorResult = 1;
                            }
                        } else {
                            pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                            pauseCtx->cursor_y[PAUSE_ITEM]++;

                            if (pauseCtx->cursor_y[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursor_y[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursor_point[PAUSE_ITEM] =
                                    pauseCtx->cursor_x[PAUSE_ITEM] + (pauseCtx->cursor_y[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursor_point[PAUSE_ITEM] >= 24) {
                                pauseCtx->cursor_point[PAUSE_ITEM] = pauseCtx->cursor_x[PAUSE_ITEM];
                            }

                            if (cursorY == pauseCtx->cursor_y[PAUSE_ITEM]) {
                                pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;
                                moveCursorResult = 2;
                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                            }
                        }
                    } else if (pauseCtx->stick_movement_x > 30) {
                        if (pauseCtx->cursor_x[PAUSE_ITEM] < 5) {
                            pauseCtx->cursor_x[PAUSE_ITEM]++;
                            pauseCtx->cursor_point[PAUSE_ITEM] += 1;

                            if(z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] != ITEM_NONE) {
                                moveCursorResult = 1;
                            }
                        } else {
                            pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                            pauseCtx->cursor_y[PAUSE_ITEM]++;

                            if (pauseCtx->cursor_y[PAUSE_ITEM] >= 4) {
                                pauseCtx->cursor_y[PAUSE_ITEM] = 0;
                            }

                            pauseCtx->cursor_point[PAUSE_ITEM] =
                                    pauseCtx->cursor_x[PAUSE_ITEM] + (pauseCtx->cursor_y[PAUSE_ITEM] * 6);

                            if (pauseCtx->cursor_point[PAUSE_ITEM] >= 24) {
                                pauseCtx->cursor_point[PAUSE_ITEM] = pauseCtx->cursor_x[PAUSE_ITEM];
                            }

                            if (cursorY == pauseCtx->cursor_y[PAUSE_ITEM]) {
                                pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);

                                moveCursorResult = 2;
                            }
                        }
                    }
                }

                if (moveCursorResult == 1) {
                    // Selected slot changed, update item reference
                    cursorItem = z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]];
                }
            }
        } else if (pauseCtx->cursor_special_pos == PAUSE_CURSOR_PAGE_LEFT) {
            // On Page Left special position going Right
            if (pauseCtx->stick_movement_x > 30) {
                pauseCtx->name_display_timer = 0;
                pauseCtx->cursor_special_pos = 0;

                z64_Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

                cursorPoint = cursorX = cursorY = 0;
                while (1) {
                    // Tries to reset state, forgets to unset cursor_item
                    if (z64_file.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                        pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                        moveCursorResult = 1;
                        break;
                    }

                    cursorY = cursorY + 1;
                    cursorPoint = cursorPoint + 6;
                    if (cursorY < 4) {
                        continue;
                    }

                    cursorY = 0;
                    cursorPoint = cursorX + 1;
                    cursorX = cursorPoint;
                    if (cursorX < 6) {
                        continue;
                    }

                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_RIGHT);
                    break;
                }
            }
        } else {
            // On Page Right special position going Left
            if (pauseCtx->stick_movement_x < -30) {
                pauseCtx->name_display_timer = 0;
                pauseCtx->cursor_special_pos = 0;

                z64_Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

                cursorPoint = cursorX = 5;
                cursorY = 0;
                while (1) {
                    // Tries to reset state, forgets to unset cursor_item
                    if (z64_file.items[cursorPoint] != ITEM_NONE) {
                        pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;
                        pauseCtx->cursor_x[PAUSE_ITEM] = cursorX;
                        pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                        moveCursorResult = 1;
                        break;
                    }

                    cursorY = cursorY + 1;
                    cursorPoint = cursorPoint + 6;
                    if (cursorY < 4) {
                        continue;
                    }

                    cursorY = 0;
                    cursorPoint = cursorX - 1;
                    cursorX = cursorPoint;
                    if (cursorX >= 0) {
                        continue;
                    }

                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    break;
                }
            }
        }

        if (pauseCtx->cursor_special_pos == 0) {
            if (cursorItem != PAUSE_ITEM_NONE) {
                if (ABS(pauseCtx->stick_movement_y) > 30) {
                    cursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];
                    cursorY = pauseCtx->cursor_y[PAUSE_ITEM];
                    moveCursorResult = 0;

                    while (moveCursorResult == 0) {
                        if (pauseCtx->stick_movement_y > 30) {
                            // Moving up
                            if (pauseCtx->cursor_y[PAUSE_ITEM] != 0) {
                                pauseCtx->cursor_y[PAUSE_ITEM]--;
                                pauseCtx->cursor_point[PAUSE_ITEM] -= 6;

                                if(z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] != ITEM_NONE) {
                                    moveCursorResult = 1;
                                }
                            } else {
                                pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        } else if (pauseCtx->stick_movement_y < -30) {
                            if (pauseCtx->cursor_y[PAUSE_ITEM] < 3) {
                                pauseCtx->cursor_y[PAUSE_ITEM]++;
                                pauseCtx->cursor_point[PAUSE_ITEM] += 6;

                                if(z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] != ITEM_NONE) {
                                    moveCursorResult = 1;
                                }
                            } else {
                                pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        }
                    }
                }
            }

            cursorSlot = pauseCtx->cursor_point[PAUSE_ITEM];

            pauseCtx->cursor_color_set = 4;

            cursorItem = z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]];

            pauseCtx->cursor_item[PAUSE_ITEM] = cursorItem;
            pauseCtx->cursor_slot[PAUSE_ITEM] = cursorSlot;

            if (!CHECK_AGE_REQ_SLOT(cursorSlot)) {
                pauseCtx->name_color_set = 1;
            }

            if (cursorItem != PAUSE_ITEM_NONE) {
                index = cursorSlot * 4;
                KaleidoScope_SetCursorVtx(pauseCtx, index, pauseCtx->item_vtx);

                if ((pauseCtx->debugState == 0) && (pauseCtx->state == PAUSE_STATE_MAIN) &&
                    (pauseCtx->changing == PAUSE_MAIN_STATE_IDLE)) {
                    if (input->pad_pressed.cl || input->pad_pressed.cd || input->pad_pressed.cr) {
                        if (CHECK_AGE_REQ_SLOT(cursorSlot) && (cursorItem != ITEM_SOLD_OUT)) {
                            if (input->pad_pressed.cl) {
                                pauseCtx->equip_target_c_btn = 0;
                            } else if (input->pad_pressed.cd) {
                                pauseCtx->equip_target_c_btn = 1;
                            } else if (input->pad_pressed.cr) {
                                pauseCtx->equip_target_c_btn = 2;
                            }

                            pauseCtx->equip_target_item = cursorItem;
                            pauseCtx->equip_target_slot = cursorSlot;
                            pauseCtx->changing = PAUSE_MAIN_STATE_3;
                            pauseCtx->equip_anim_x = pauseCtx->item_vtx[index].v.ob[0] * 10;
                            pauseCtx->equip_anim_y = pauseCtx->item_vtx[index].v.ob[1] * 10;
                            pauseCtx->equip_anim_alpha = 255;
                            sEquipAnimTimer = 0;
                            sEquipState = 3;
                            sEquipMoveTimer = 10;
                            if ((pauseCtx->equip_target_item == ITEM_ARROW_FIRE) ||
                                (pauseCtx->equip_target_item == ITEM_ARROW_ICE) ||
                                (pauseCtx->equip_target_item == ITEM_ARROW_LIGHT)) {
                                index = 0;
                                if (pauseCtx->equip_target_item == ITEM_ARROW_ICE) {
                                    index = 1;
                                }
                                if (pauseCtx->equip_target_item == ITEM_ARROW_LIGHT) {
                                    index = 2;
                                }
                                z64_Audio_PlaySoundGeneral(NA_SE_SY_SET_FIRE_ARROW + index, &gSfxDefaultPos, 4,
                                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale,
                                                     &gSfxDefaultReverb);
                                pauseCtx->equip_target_item = 0xBF + index;
                                sEquipState = 0;
                                pauseCtx->equip_anim_alpha = 0;
                                sEquipMoveTimer = 6;
                            } else {
                                z64_Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                            }
                        } else {
                            z64_Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                        }
                    }
                }
            } else {
                pauseCtx->cursor_vtx[0].v.ob[0] = pauseCtx->cursor_vtx[2].v.ob[0] = pauseCtx->cursor_vtx[1].v.ob[0] =
                pauseCtx->cursor_vtx[3].v.ob[0] = 0;

                pauseCtx->cursor_vtx[0].v.ob[1] = pauseCtx->cursor_vtx[1].v.ob[1] = pauseCtx->cursor_vtx[2].v.ob[1] =
                pauseCtx->cursor_vtx[3].v.ob[1] = -200;
            }
        } else {
            pauseCtx->cursor_item[PAUSE_ITEM] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursor_point[PAUSE_ITEM]) {
            z64_Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                 &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
        }
    } else if ((pauseCtx->changing == PAUSE_MAIN_STATE_3) && (pauseCtx->screen_idx == PAUSE_ITEM)) {
        KaleidoScope_SetCursorVtx(pauseCtx, cursorSlot * 4, pauseCtx->item_vtx);
        pauseCtx->cursor_color_set = 4;
    }


    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    // Draw outline around equipped items
    for (i = 0, j = 24 * 4; i < 3; i++, j += 4) {
        if (z64_file.button_items[i + 1] != ITEM_NONE) {
            gSPVertex(POLY_OPA_DISP++ , &pauseCtx->item_vtx[j], 4, 0);
            POLY_OPA_DISP = KaleidoScope_QuadTextureIA8(POLY_OPA_DISP, gEquippedItemOutlineTex, 32, 32, 0);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    // Draw the item icons in the menu
    for (i = j = 0; i < 24; i++, j += 4) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (z64_file.items[i] != ITEM_NONE) { // Only if you've obtained it
            if ((pauseCtx->changing == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->screen_idx == PAUSE_ITEM) &&
                (pauseCtx->cursor_special_pos == 0)) { // Cursor is over an item
                if (CHECK_AGE_REQ_SLOT(i)) { // Item can be equipped as current age
                    if ((sEquipState == 2) && (i == 3)) { // Currently equipping a bow item?
                        // This doesn't seem to do anything?
                        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, magicArrowEffectsR[pauseCtx->equip_target_item - 0xBF],
                                        magicArrowEffectsG[pauseCtx->equip_target_item - 0xBF],
                                        magicArrowEffectsB[pauseCtx->equip_target_item - 0xBF], pauseCtx->alpha);

                        pauseCtx->item_vtx[j + 0].v.ob[0] = pauseCtx->item_vtx[j + 2].v.ob[0] =
                                pauseCtx->item_vtx[j + 0].v.ob[0] - 2;

                        pauseCtx->item_vtx[j + 1].v.ob[0] = pauseCtx->item_vtx[j + 3].v.ob[0] =
                                pauseCtx->item_vtx[j + 0].v.ob[0] + 32;

                        pauseCtx->item_vtx[j + 0].v.ob[1] = pauseCtx->item_vtx[j + 1].v.ob[1] =
                                pauseCtx->item_vtx[j + 0].v.ob[1] + 2;

                        pauseCtx->item_vtx[j + 2].v.ob[1] = pauseCtx->item_vtx[j + 3].v.ob[1] =
                                pauseCtx->item_vtx[j + 0].v.ob[1] - 32;
                    } else if (i == cursorSlot) { // Draw the item the cursor is over slightly larger
                        pauseCtx->item_vtx[j + 0].v.ob[0] = pauseCtx->item_vtx[j + 2].v.ob[0] =
                                pauseCtx->item_vtx[j + 0].v.ob[0] - 2;

                        pauseCtx->item_vtx[j + 1].v.ob[0] = pauseCtx->item_vtx[j + 3].v.ob[0] =
                                pauseCtx->item_vtx[j + 0].v.ob[0] + 32;

                        pauseCtx->item_vtx[j + 0].v.ob[1] = pauseCtx->item_vtx[j + 1].v.ob[1] =
                                pauseCtx->item_vtx[j + 0].v.ob[1] + 2;

                        pauseCtx->item_vtx[j + 2].v.ob[1] = pauseCtx->item_vtx[j + 3].v.ob[1] =
                                pauseCtx->item_vtx[j + 0].v.ob[1] - 32;
                    }
                }
            }

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->item_vtx[j + 0], 4, 0);
            KaleidoScope_DrawQuadTextureRGBA32(play->common.gfx,
                                               gItemIcons[z64_file.items[i]], ITEM_ICON_WIDTH,
                                               ITEM_ICON_HEIGHT, 0);
        }
    }

    if (pauseCtx->cursor_special_pos == 0) {
        KaleidoScope_DrawCursor(play, PAUSE_ITEM);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    // Draw the current ammo amounts
    for (i = 0; i < 15; i++) {
        if ((gAmmoItems[i] != ITEM_NONE) && (z64_file.items[i] != ITEM_NONE)) {
            KaleidoScope_DrawAmmoCount(pauseCtx, play->common.gfx, z64_file.items[i]);
        }
    }

    CLOSE_DISPS(play->common.gfx);
}