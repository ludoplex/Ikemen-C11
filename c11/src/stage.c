/* stage.c - Stage/background system implementation
 * C11 analog of src/stage.go (4,892 lines)
 * Handles stage definition, backgrounds, camera, and parallax
 */

#include "ikemen_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct ikm_stage {
    char* name;
    char* author;
    char* displayname;
    int32_t zoffset;
    float scale[2];
    float bound_left, bound_right, bound_high, bound_low;
    float tension_high, tension_low;
    float verticalfollow;
    int32_t overdrawhigh, overdrawlow;
    float cuthigh, cutlow;
    float startzoom;
    float zoomin, zoomout;
    ikm_camera_t* camera;
    ikm_bgdef_t** bg_layers;
    size_t bg_layer_count;
    ikm_sprite_cache_t* sprites;
    ikm_animation_t* anims;
};

ikm_stage_t* ikm_stage_create(const char* name) {
    ikm_stage_t* stage = calloc(1, sizeof(ikm_stage_t));
    if (!stage) return NULL;
    
    if (name) {
        stage->name = strdup(name);
        if (!stage->name) {
            free(stage);
            return NULL;
        }
    }
    
    /* Default values matching stage.go */
    stage->zoffset = 0;
    stage->scale[0] = 1.0f;
    stage->scale[1] = 1.0f;
    stage->bound_left = -1000.0f;
    stage->bound_right = 1000.0f;
    stage->bound_high = 0.0f;
    stage->bound_low = 0.0f;
    stage->startzoom = 1.0f;
    stage->zoomin = 1.0f;
    stage->zoomout = 1.0f;
    
    stage->camera = ikm_camera_create();
    
    return stage;
}

void ikm_stage_destroy(ikm_stage_t* stage) {
    if (!stage) return;
    
    free(stage->name);
    free(stage->author);
    free(stage->displayname);
    ikm_camera_destroy(stage->camera);
    
    for (size_t i = 0; i < stage->bg_layer_count; i++) {
        ikm_bgdef_destroy(stage->bg_layers[i]);
    }
    free(stage->bg_layers);
    
    free(stage);
}

bool ikm_stage_load_def(ikm_stage_t* stage, const char* def_path) {
    if (!stage || !def_path) return false;
    
    ikm_ini_file_t* ini = ikm_ini_load(def_path);
    if (!ini) return false;
    
    /* Parse [Info] section */
    const char* name = ikm_ini_get_string(ini, "Info", "name", "");
    if (*name) {
        free(stage->name);
        stage->name = strdup(name);
    }
    
    const char* author = ikm_ini_get_string(ini, "Info", "author", "");
    if (*author) {
        free(stage->author);
        stage->author = strdup(author);
    }
    
    /* Parse [Camera] section */
    stage->bound_left = ikm_ini_get_float(ini, "Camera", "boundleft", -1000.0f);
    stage->bound_right = ikm_ini_get_float(ini, "Camera", "boundright", 1000.0f);
    stage->bound_high = ikm_ini_get_float(ini, "Camera", "boundhigh", 0.0f);
    stage->bound_low = ikm_ini_get_float(ini, "Camera", "boundlow", 0.0f);
    stage->startzoom = ikm_ini_get_float(ini, "Camera", "startzoom", 1.0f);
    
    /* Parse [Scaling] section */
    stage->scale[0] = ikm_ini_get_float(ini, "Scaling", "topscale", 1.0f);
    stage->scale[1] = ikm_ini_get_float(ini, "Scaling", "botscale", 1.0f);
    
    /* Load background definitions */
    /* This would parse [BGDef] and [BG *] sections */
    
    ikm_ini_free(ini);
    return true;
}

void ikm_stage_update(ikm_stage_t* stage, float camera_x, float camera_y) {
    if (!stage) return;
    
    /* Update camera */
    ikm_camera_update(stage->camera, camera_x, camera_y);
    
    /* Update background layers */
    for (size_t i = 0; i < stage->bg_layer_count; i++) {
        ikm_bgdef_update(stage->bg_layers[i], stage->camera);
    }
}

void ikm_stage_draw(ikm_stage_t* stage, ikm_renderer_t* renderer) {
    if (!stage || !renderer) return;
    
    /* Draw background layers in order */
    for (size_t i = 0; i < stage->bg_layer_count; i++) {
        ikm_bgdef_draw(stage->bg_layers[i], renderer);
    }
}

const char* ikm_stage_get_name(ikm_stage_t* stage) {
    return stage ? stage->name : "";
}

void ikm_stage_get_bounds(ikm_stage_t* stage, float* left, float* right, 
                          float* high, float* low) {
    if (!stage) return;
    if (left) *left = stage->bound_left;
    if (right) *right = stage->bound_right;
    if (high) *high = stage->bound_high;
    if (low) *low = stage->bound_low;
}
