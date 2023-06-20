#include "invertYaxis.h"

extern uint8_t CFG_INVERT_YAXIS_IN_FIRST_PERSON_CAMERA;

void manage_invert_yaxis() {

    if (CFG_INVERT_YAXIS_IN_FIRST_PERSON_CAMERA) {
        // Never if a choice textbox is on screen.
        if (z64_MessageGetState(((uint8_t *)(&z64_game)) + 0x20D8) == 4) {
            return;
        }
        if (z64_game.camera_mode == 0x06 ||
            z64_game.camera_mode == 0x07 ||
            z64_game.camera_mode == 0x0A ||
            z64_game.camera_mode == 0x0B) {
            z64_game.common.input[0].raw.y = -z64_game.common.input[0].raw.y;
            z64_game.common.input[0].y_diff = -z64_game.common.input[0].y_diff;
            z64_game.common.input[0].adjusted_y = -z64_game.common.input[0].adjusted_y;
        }
    }
}
