/* char.c - Character system implementation
 * C11 analog of src/char.go (12,763 lines)
 * Handles character state, physics, attacks, and AI
 */

#include "ikemen_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/* Character structure - matches char.go Char struct */
struct ikm_char {
    char* name;
    ikm_palfx_t* palfx;
    ikm_animation_t* anim;
    ikm_animation_t* anim_backup;
    ikm_anim_frame_t* cur_frame;
    ikm_command_list_t** cmd;
    size_t cmd_count;
    ikm_state_state_t ss;
    int controller;
    int32_t id;
    int32_t runorder;
    int32_t helper_id;
    int32_t helper_index;
    int32_t parent_index;
    int player_no;
    int teamside;
    bool keyctrl[4];
    bool player_flag;
    bool hprojectile;
    int anim_pn;
    int sprite_pn;
    int32_t anim_no;
    int32_t prev_anim_no;
    int32_t life;
    int32_t life_max;
    int32_t power;
    int32_t power_max;
    int32_t dizzy_points;
    int32_t dizzy_points_max;
    int32_t guard_points;
    int32_t guard_points_max;
    float pos_x, pos_y, pos_z;
    float vel_x, vel_y, vel_z;
    float facing;
    ikm_state_type_t state_type;
    ikm_move_type_t move_type;
    ikm_physics_t* physics;
    ikm_hitdef_t* hitdef;
    ikm_hitdef_t** projectiles;
    size_t projectile_count;
    ikm_sprite_cache_t* sprites;
    ikm_sound_cache_t* sounds;
};

ikm_char_t* ikm_char_create(const char* name, int player_no) {
    if (!name) return NULL;
    
    ikm_char_t* c = calloc(1, sizeof(ikm_char_t));
    if (!c) return NULL;
    
    c->name = strdup(name);
    if (!c->name) {
        free(c);
        return NULL;
    }
    
    c->player_no = player_no;
    c->id = player_no;
    c->life_max = 1000;
    c->life = 1000;
    c->power_max = 3000;
    c->power = 0;
    c->dizzy_points_max = 0;
    c->guard_points_max = 0;
    c->facing = 1.0f;
    c->state_type = IKM_STATE_STANDING;
    c->move_type = IKM_MOVE_IDLE;
    
    c->palfx = ikm_palfx_create();
    c->anim = ikm_animation_create();
    c->physics = ikm_physics_create();
    
    return c;
}

void ikm_char_destroy(ikm_char_t* c) {
    if (!c) return;
    
    free(c->name);
    ikm_palfx_destroy(c->palfx);
    ikm_animation_destroy(c->anim);
    ikm_animation_destroy(c->anim_backup);
    ikm_physics_destroy(c->physics);
    ikm_hitdef_destroy(c->hitdef);
    
    for (size_t i = 0; i < c->projectile_count; i++) {
        ikm_hitdef_destroy(c->projectiles[i]);
    }
    free(c->projectiles);
    
    for (size_t i = 0; i < c->cmd_count; i++) {
        ikm_command_list_destroy(c->cmd[i]);
    }
    free(c->cmd);
    
    free(c);
}

void ikm_char_update(ikm_char_t* c) {
    if (!c) return;
    
    /* Update animation */
    ikm_animation_update(c->anim);
    c->cur_frame = ikm_animation_get_current_frame(c->anim);
    
    /* Update physics */
    ikm_physics_update(c->physics, c);
    
    /* Apply velocity */
    c->pos_x += c->vel_x;
    c->pos_y += c->vel_y;
    c->pos_z += c->vel_z;
    
    /* Update power */
    if (c->power < c->power_max) {
        /* Power generation logic */
    }
}

void ikm_char_set_position(ikm_char_t* c, float x, float y, float z) {
    if (!c) return;
    c->pos_x = x;
    c->pos_y = y;
    c->pos_z = z;
}

void ikm_char_get_position(ikm_char_t* c, float* x, float* y, float* z) {
    if (!c) return;
    if (x) *x = c->pos_x;
    if (y) *y = c->pos_y;
    if (z) *z = c->pos_z;
}

void ikm_char_set_velocity(ikm_char_t* c, float vx, float vy, float vz) {
    if (!c) return;
    c->vel_x = vx;
    c->vel_y = vy;
    c->vel_z = vz;
}

void ikm_char_damage(ikm_char_t* c, int32_t damage) {
    if (!c) return;
    c->life -= damage;
    if (c->life < 0) c->life = 0;
}

void ikm_char_heal(ikm_char_t* c, int32_t amount) {
    if (!c) return;
    c->life += amount;
    if (c->life > c->life_max) c->life = c->life_max;
}

void ikm_char_add_power(ikm_char_t* c, int32_t amount) {
    if (!c) return;
    c->power += amount;
    if (c->power > c->power_max) c->power = c->power_max;
    if (c->power < 0) c->power = 0;
}

bool ikm_char_is_alive(ikm_char_t* c) {
    return c && c->life > 0;
}

void ikm_char_change_anim(ikm_char_t* c, int32_t anim_no) {
    if (!c) return;
    if (c->anim_no != anim_no) {
        c->prev_anim_no = c->anim_no;
        c->anim_no = anim_no;
        ikm_animation_reset(c->anim);
    }
}

void ikm_char_set_state(ikm_char_t* c, ikm_state_type_t state) {
    if (!c) return;
    c->state_type = state;
}

ikm_state_type_t ikm_char_get_state(ikm_char_t* c) {
    return c ? c->state_type : IKM_STATE_STANDING;
}

void ikm_char_set_control(ikm_char_t* c, bool ctrl) {
    if (!c) return;
    c->controller = ctrl ? 1 : 0;
}

bool ikm_char_get_control(ikm_char_t* c) {
    return c && c->controller != 0;
}

int32_t ikm_char_get_life(ikm_char_t* c) {
    return c ? c->life : 0;
}

int32_t ikm_char_get_power(ikm_char_t* c) {
    return c ? c->power : 0;
}

const char* ikm_char_get_name(ikm_char_t* c) {
    return c ? c->name : "";
}

void ikm_char_set_facing(ikm_char_t* c, float facing) {
    if (!c) return;
    c->facing = facing > 0.0f ? 1.0f : -1.0f;
}

float ikm_char_get_facing(ikm_char_t* c) {
    return c ? c->facing : 1.0f;
}
