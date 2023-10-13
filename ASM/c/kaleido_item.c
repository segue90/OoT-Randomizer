#include "audio.h"
#include "kaleido_item.h"

uint8_t* gSlotAgeReqs;

#define AGE_REQ_NONE 9
#define CHECK_AGE_REQ_SLOT(slot) \
    ((gSlotAgeReqs[slot] == AGE_REQ_NONE) || gSlotAgeReqs[slot] == ((void)0, z64_file.link_age))
void* gItemIcons;
void* gEquippedItemOutlineTex;

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

//    z64_disp_buf_t *db_opa = &(z64_ctxt.gfx->poly_opa);
    z64_disp_buf_t *db_opa = &(play->common.gfx->poly_opa);
    z64_disp_buf_t *db_ovl = &(play->common.gfx->overlay);
    Gfx *orig_opa;
    Gfx *orig_ovl;
    {
        (void) play->common.gfx;
        orig_opa = db_opa->p;
        orig_ovl = db_ovl->p;
        db_opa->p++;
        db_ovl->p++;
    }// OPEN_DISPS(play->common.gfx, "../z_kaleido_item.c", 234);

    {
        z64_disp_buf_t *db_opa_ = &(play->common.gfx->poly_opa);
        (void) play->common.gfx;
        Gfx *orig_opa_ = db_opa_->p;
        db_opa_->p++;

        gsDPPipeSync();
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2);
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
        gsSPEndDisplayList();

        db_opa_->p = orig_opa_;
    }// Gfx_SetupDL_42Opa(play->common.gfx);

        gDPSetCombineMode(db_opa->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    pauseCtx->cursor_color_set = 0;
    pauseCtx->name_color_set = 0;

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->changing == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->screen_idx == PAUSE_ITEM)) {
        moveCursorResult = 0;
        oldCursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];

        cursorItem = pauseCtx->cursor_item[PAUSE_ITEM];
        cursorSlot = pauseCtx->cursor_slot[PAUSE_ITEM];

        if (pauseCtx->cursor_special_pos == 0) {
            pauseCtx->cursor_color_set = 4;

            if (ABS(pauseCtx->stick_movement_x) > 30) {

                if (cursorItem == PAUSE_ITEM_NONE) {
                    pauseCtx->stick_movement_x = 40;
                }
                cursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];
                cursorX = pauseCtx->cursor_x[PAUSE_ITEM];
                cursorY = pauseCtx->cursor_y[PAUSE_ITEM];

                // Seems necessary to match
                if (pauseCtx->cursor_x[PAUSE_ITEM]) {}
                if (z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]]) {}

                while (moveCursorResult == 0) {
                    if (pauseCtx->stick_movement_x < -30) {
                        if (pauseCtx->cursor_x[PAUSE_ITEM] != 0) {
                            pauseCtx->cursor_x[PAUSE_ITEM]--;
                            pauseCtx->cursor_point[PAUSE_ITEM] -= 1;

                            // if (z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] !=
                            //     ITEM_NONE) {
                            moveCursorResult = 1;
                            // }
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

                                KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);

                                moveCursorResult = 2;
                            }
                        }
                    } else if (pauseCtx->stick_movement_x > 30) {
                        if (pauseCtx->cursor_x[PAUSE_ITEM] < 5) {
                            pauseCtx->cursor_x[PAUSE_ITEM]++;
                            pauseCtx->cursor_point[PAUSE_ITEM] += 1;

                            // if (z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] !=
                            //     ITEM_NONE) {
                            moveCursorResult = 1;
                            // }
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
//                    cursorItem = gSaveContext.save.info.inventory.items[pauseCtx->cursor_point[PAUSE_ITEM]];
                      cursorItem = z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]];
                }
            }
        } else if (pauseCtx->cursor_special_pos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stick_movement_x > 30) {
                pauseCtx->name_display_timer = 0;
                pauseCtx->cursor_special_pos = 0;

                z64_Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

                cursorPoint = cursorX = cursorY = 0;
                while (1) {
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
            if (pauseCtx->stick_movement_x < -30) {
                pauseCtx->name_display_timer = 0;
                pauseCtx->cursor_special_pos = 0;

                //FUN_800c806c(0x4809,0xffffffff80104394,4,0xffffffff801043a0,&DAT_801043a0,&DAT_801043a8)
                //z64_Audio_PlaySoundGeneral(sfxId, (void *)0x80104394, 4, (float *)0x801043A0, (float *)0x801043A0, (uint8_t *)0x801043A8);
                z64_Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
//                Audio_PlaySfxGeneral(NA_SE_SY_CURSOR, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
//                                     &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

                cursorPoint = cursorX = 5;
                cursorY = 0;
                while (1) {
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

                    //FUN_8081f184
                    KaleidoScope_MoveCursorToSpecialPos(play, PAUSE_CURSOR_PAGE_LEFT);
                    break;
                }
            }
        }

        if (pauseCtx->cursor_special_pos == 0) {
            if (cursorItem != PAUSE_ITEM_NONE) {
                if (ABS(pauseCtx->stick_movement_y) > 30) {
                    moveCursorResult = 0;

                    cursorPoint = pauseCtx->cursor_point[PAUSE_ITEM];
                    cursorY = pauseCtx->cursor_y[PAUSE_ITEM];
                    while (moveCursorResult == 0) {
                        if (pauseCtx->stick_movement_y > 30) {
                            if (pauseCtx->cursor_y[PAUSE_ITEM] != 0) {
                                pauseCtx->cursor_y[PAUSE_ITEM]--;
                                pauseCtx->cursor_point[PAUSE_ITEM] -= 6;

                                // if (z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] !=
                                //     ITEM_NONE) {
                                moveCursorResult = 1;
                                // }
                            } else {
                                pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        } else if (pauseCtx->stick_movement_y < -30) {
                            if (pauseCtx->cursor_y[PAUSE_ITEM] < 3) {
                                pauseCtx->cursor_y[PAUSE_ITEM]++;
                                pauseCtx->cursor_point[PAUSE_ITEM] += 6;

                                // if (z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]] !=
                                //     ITEM_NONE) {
                                moveCursorResult = 1;
                                // }
                            } else {
                                pauseCtx->cursor_y[PAUSE_ITEM] = cursorY;
                                pauseCtx->cursor_point[PAUSE_ITEM] = cursorPoint;

                                moveCursorResult = 2;
                            }
                        }
                    }

                    cursorPoint = PAUSE_ITEM;
                }
            }

            cursorSlot = pauseCtx->cursor_point[PAUSE_ITEM];

            pauseCtx->cursor_color_set = 4;

            if (moveCursorResult != 2) {
                cursorItem = z64_file.items[pauseCtx->cursor_point[PAUSE_ITEM]];
            }

            pauseCtx->cursor_item[PAUSE_ITEM] = cursorItem;
            pauseCtx->cursor_slot[PAUSE_ITEM] = cursorSlot;

            if (!CHECK_AGE_REQ_SLOT(cursorSlot)) {
                pauseCtx->name_color_set = 1;
            }

            if (cursorItem != PAUSE_ITEM_NONE) {
                index = cursorSlot * 4; // required to match?
                //FUN_80819bf4
                KaleidoScope_SetCursorVtx(pauseCtx, index, pauseCtx->item_vtx);

                if ((pauseCtx->state == PAUSE_STATE_MAIN) &&
                    (pauseCtx->changing == PAUSE_MAIN_STATE_IDLE)) {
                    if (input->pad_pressed.cl || input->pad_pressed.cd || input->pad_pressed.cr) {
//                    if (CHECK_BTN_ANY(input->press.button, BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT)) {
                        if (CHECK_AGE_REQ_SLOT(cursorSlot) && (cursorItem != ITEM_SOLD_OUT) && (cursorItem != ITEM_NONE)) {
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
        //FUN_80819bf4
        KaleidoScope_SetCursorVtx(pauseCtx, cursorSlot * 4, pauseCtx->item_vtx);
        pauseCtx->cursor_color_set = 4;
    }

    gDPSetCombineLERP(db_ovl->p++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(db_opa->p++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetEnvColor(db_opa->p++, 0, 0, 0, 0);

    for (i = 0, j = 24 * 4; i < 3; i++, j += 4) {
        if (z64_file.button_items[i + 1] != ITEM_NONE) {
//        if (gSaveContext.save.info.equips.buttonItems[i + 1] != ITEM_NONE) {
            gSPVertex(db_opa->p++, &pauseCtx->item_vtx[j], 4, 0);
            //FUN_8081EE60 -- gEquippedItemOutlineTex = 0x2000e00 Not pointer in ghidra??
            db_opa->p = KaleidoScope_QuadTextureIA8(db_opa->p, gEquippedItemOutlineTex, 32, 32, 0);
        }
    }

    gDPPipeSync(db_opa->p++);
    gDPSetCombineMode(db_opa->p++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (i = j = 0; i < 24; i++, j += 4) {
        gDPSetPrimColor(db_opa->p++, 0, 0, 255, 255, 255, pauseCtx->alpha);

        if (z64_file.items[i] != ITEM_NONE) {
            if ((pauseCtx->changing == PAUSE_MAIN_STATE_IDLE) && (pauseCtx->screen_idx == PAUSE_ITEM) &&
                (pauseCtx->cursor_special_pos == 0)) {
                if (CHECK_AGE_REQ_SLOT(i)) {
                    if ((sEquipState == 2) && (i == 3)) {
                        gDPSetPrimColor(db_opa->p++, 0, 0, magicArrowEffectsR[pauseCtx->equip_target_item - 0xBF],
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
                    } else if (i == cursorSlot) {
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

            gSPVertex(db_opa->p++, &pauseCtx->item_vtx[j + 0], 4, 0);
            //FUN_8081f1e8
            KaleidoScope_DrawQuadTextureRGBA32(play->common.gfx,
                                               &gItemIcons[z64_file.items[i]], ITEM_ICON_WIDTH,
                                               ITEM_ICON_HEIGHT, 0);
        }
    }

    if (pauseCtx->cursor_special_pos == 0) {
        //FUN_8081f760
        KaleidoScope_DrawCursor(play, PAUSE_ITEM);
    }

    gDPPipeSync(db_opa->p++);
    gDPSetCombineLERP(db_opa->p++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    for (i = 0; i < 15; i++) {
        if ((gAmmoItems[i] != ITEM_NONE) && (z64_file.items[i] != ITEM_NONE)) {
            //FUN_80819670
            KaleidoScope_DrawAmmoCount(pauseCtx, play->common.gfx, z64_file.items[i]);
        }
    }

    {
        db_opa->p = orig_opa;
        db_ovl->p = orig_ovl;
    } //CLOSE_DISPS(play->common.gfx, "../z_kaleido_item.c", 516);
}