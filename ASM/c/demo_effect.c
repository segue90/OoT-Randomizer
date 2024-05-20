#include "z64.h"
#include "util.h"
#include "demo_effect.h"
#include "models.h"

extern void DemoEffect_DrawJewel(z64_actor_t* this, z64_game_t* globalCtx, void* func);
extern void* DemoEffect_DrawJewel_AfterHook;

void DemoEffect_DrawJewel_Hook(DemoEffect* this, z64_game_t* globalCtx) {
    if (!this->override_initialized) {
        if (globalCtx->scene_index == 0x02) {
            override_key_t sapphire_override_key = {
                .scene = 0x13,
                .type = 0x00,
                .flag = 0x65,
            };
            this->override = lookup_override_by_key(sapphire_override_key);
            if (this->override.key.all) {
                this->actor.rot_2.x = 0;
            }
        }

        this->override_initialized = true;
    }
    if (this->override.key.all) {
        model_t model;
        lookup_model_by_override(&model, this->override);
        draw_model(model, &this->actor, globalCtx, 5.0);
    } else {
        DemoEffect_DrawJewel(this, globalCtx, resolve_overlay_addr(&DemoEffect_DrawJewel_AfterHook, 0x008B));
    }
}
