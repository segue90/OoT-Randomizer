#include "triforce.h"
#include "model_text.h"
#include "dungeon_info.h"
#include "file_select.h"
#include "get_items.h"
#include "models.h"
#include "gfx.h"
#include "text.h"
#include "util.h"
#include "dpad.h"
#include "misc_colors.h"
#include "hud_colors.h"
#include "z64.h"
#include "chests.h"
#include "ganon.h"
#include "twinrova.h"
#include "sage_gifts.h"
#include "extern_ctxt.h"
#include "weather.h"
#include "textures.h"
#include "scene.h"
#include "music.h"
#include "uninvertYaxis.h"
#include "debug.h"
#include "inputviewer.h"
#include "message.h"

z64_disp_buf_t rando_display_buffer_mem __attribute__ ((aligned (16)));
z64_disp_buf_t* rando_db __attribute__ ((aligned (16)));
Gfx rando_dl_buffer[0x1000] __attribute__ ((aligned (16)));

void Gameplay_InitSkybox(z64_game_t* globalCtx, int16_t skyboxId);

void c_init() {
    heap_init();
    gfx_init();
    text_init();
    item_overrides_init();
    override_flags_init();
    models_init();
    init_textures();
    rando_db = &rando_display_buffer_mem;
    rando_db->buf = rando_dl_buffer;
    rando_db->p = &rando_db->buf[0];
}

void before_game_state_update() {
    handle_pending_items();
    handle_dpad();
    update_misc_colors();
    update_hud_colors();
    process_extern_ctxt();
    manage_music_changes();
    manage_uninvert_yaxis();
    display_misc_messages();
}

void after_game_state_update() {
    OPEN_DISPS(z64_ctxt.gfx);

    rando_db->p = &rando_dl_buffer[0];
    // Checks if the prerender screen is being drawn before drawing new HUD things.
    // Else this will cause graphical and/or lag issues on some emulators when pausing.
    if (R_PAUSE_BG_PRERENDER_STATE != PAUSE_BG_PRERENDER_PROCESS) {

        draw_dungeon_info(rando_db);
        draw_triforce_count(rando_db);
        draw_boss_key(&z64_game, rando_db);
        draw_silver_rupee_count(&z64_game, rando_db);
        draw_illegal_model_text(rando_db);
        draw_input_viewer(rando_db);
        display_song_name(rando_db);
        debug_utilities(rando_db);
    }
    give_sage_gifts();

    gSPEndDisplayList(rando_db->p++);
    gSPDisplayList(POLY_OPA_DISP++, &rando_db->buf[0]);

    CLOSE_DISPS();
}

void before_skybox_init(z64_game_t* game, int16_t skyboxId) {
    override_weather_state();
    Gameplay_InitSkybox(game, skyboxId);
}

void after_scene_init() {
    check_ganon_entry();
    clear_twinrova_vars();
    models_reset();
    extern_scene_init();
    check_model_skeletons();
    reset_collectible_mutex();
    get_current_scene_setup_number();
}
