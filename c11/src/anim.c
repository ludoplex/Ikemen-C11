/* anim.c - Animation system (C11 analog of anim.go)
 * Implements AIR format animation parsing and playback.
 * Handles sprite animation frames, interpolation, collision boxes (CLSN).
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

/* === Trans (Transparency) Types === */
typedef enum {
    IKM_TT_NONE = 0,
    IKM_TT_ADD,
    IKM_TT_SUB,
    IKM_TT_DEFAULT
} ikm_trans_type_t;

/* === Animation Frame === */
typedef struct {
    float clsn1[4]; /* Collision box 1: left, top, right, bottom */
    float clsn2[4]; /* Collision box 2 */
} ikm_clsn_box_t;

struct ikm_anim_frame {
    int32_t time;
    int16_t group;
    int16_t number;
    int16_t xoffset;
    int16_t yoffset;
    ikm_trans_type_t trans_type;
    uint8_t src_alpha;
    uint8_t dst_alpha;
    int8_t hscale;  /* -1 for flip, 1 for normal */
    int8_t vscale;  /* -1 for flip, 1 for normal */
    float xscale;
    float yscale;
    float angle;
    ikm_clsn_box_t* clsn1;
    size_t clsn1_count;
    ikm_clsn_box_t* clsn2;
    size_t clsn2_count;
};

ikm_anim_frame_t* ikm_anim_frame_create(void) {
    ikm_anim_frame_t* af = calloc(1, sizeof(ikm_anim_frame_t));
    if (!af) return NULL;
    
    /* Initialize defaults (matches Go newAnimFrame) */
    af->time = -1;
    af->group = -1;
    af->trans_type = IKM_TT_NONE;
    af->src_alpha = 255;
    af->dst_alpha = 0;
    af->hscale = 1;
    af->vscale = 1;
    af->xscale = 1.0f;
    af->yscale = 1.0f;
    af->angle = 0.0f;
    
    return af;
}

void ikm_anim_frame_destroy(ikm_anim_frame_t* af) {
    if (!af) return;
    free(af->clsn1);
    free(af->clsn2);
    free(af);
}

/* Parse animation frame from AIR format line */
ikm_anim_frame_t* ikm_anim_frame_parse(const char* line) {
    if (!line || !*line) return NULL;
    if (!isdigit((unsigned char)line[0]) && line[0] != '-') return NULL;
    
    ikm_anim_frame_t* af = ikm_anim_frame_create();
    if (!af) return NULL;
    
    /* Parse: group, number, xoffset, yoffset, time [,flags [,alpha [,xscale [,yscale [,angle]]]]] */
    char buf[1024];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    char* parts[10] = {NULL};
    int part_count = 0;
    
    /* Split by comma */
    char* ptr = buf;
    char* start = buf;
    while (*ptr && part_count < 10) {
        if (*ptr == ',') {
            *ptr = '\0';
            parts[part_count++] = start;
            start = ptr + 1;
        }
        ptr++;
    }
    if (*start && part_count < 10) {
        parts[part_count++] = start;
    }
    
    if (part_count < 5) {
        ikm_anim_frame_destroy(af);
        return NULL;
    }
    
    /* Parse required fields */
    af->group = (int16_t)atoi(parts[0]);
    af->number = (int16_t)atoi(parts[1]);
    af->xoffset = (int16_t)atoi(parts[2]);
    af->yoffset = (int16_t)atoi(parts[3]);
    af->time = atoi(parts[4]);
    
    /* Parse flags (H/V flip) */
    if (part_count >= 6 && parts[5]) {
        for (const char* p = parts[5]; *p; p++) {
            if (*p == 'H' || *p == 'h') {
                af->hscale = -1;
                af->xoffset *= -1;
            } else if (*p == 'V' || *p == 'v') {
                af->vscale = -1;
                af->yoffset *= -1;
            }
        }
    }
    
    /* Parse alpha blending */
    if (part_count >= 7 && parts[6]) {
        char* alpha_str = parts[6];
        /* Skip to A or S */
        while (*alpha_str && *alpha_str != 'A' && *alpha_str != 'a' && 
               *alpha_str != 'S' && *alpha_str != 's') {
            alpha_str++;
        }
        
        if (*alpha_str) {
            char mode[16] = {0};
            int i = 0;
            while (*alpha_str && i < 15) {
                mode[i++] = tolower((unsigned char)*alpha_str);
                alpha_str++;
            }
            mode[i] = '\0';
            
            if (strcmp(mode, "a1") == 0) {
                af->trans_type = IKM_TT_ADD;
                af->src_alpha = 255;
                af->dst_alpha = 128;
            } else if (strncmp(mode, "as", 2) == 0) {
                af->trans_type = IKM_TT_ADD;
                af->src_alpha = (uint8_t)atoi(mode + 2);
                /* Parse dest alpha if present */
                char* d = strchr(mode, 'd');
                if (d) {
                    af->dst_alpha = (uint8_t)atoi(d + 1);
                } else {
                    af->dst_alpha = 255;
                }
            } else if (mode[0] == 'a') {
                af->trans_type = IKM_TT_ADD;
                af->src_alpha = 255;
                af->dst_alpha = 255;
            } else if (strncmp(mode, "ss", 2) == 0) {
                af->trans_type = IKM_TT_SUB;
                af->src_alpha = (uint8_t)atoi(mode + 2);
                char* d = strchr(mode, 'd');
                if (d) {
                    af->dst_alpha = (uint8_t)atoi(d + 1);
                } else {
                    af->dst_alpha = 255;
                }
            } else if (mode[0] == 's') {
                af->trans_type = IKM_TT_SUB;
                af->src_alpha = 255;
                af->dst_alpha = 255;
            }
        }
    }
    
    /* Parse X scale */
    if (part_count >= 8 && parts[7] && *parts[7]) {
        char *endptr = NULL;
        double val = strtod(parts[7], &endptr);
        if (endptr != parts[7] && *endptr == '\0') {
            af->xscale = (float)val;
        } else {
            af->xscale = 1.0f; /* default value */
        }
    }
    
    /* Parse Y scale */
    if (part_count >= 9 && parts[8] && *parts[8]) {
        char *endptr = NULL;
        double val = strtod(parts[8], &endptr);
        if (endptr != parts[8] && *endptr == '\0') {
            af->yscale = (float)val;
        } else {
            af->yscale = 1.0f; /* default value */
        }
    }
    
    /* Parse angle */
    if (part_count >= 10 && parts[9] && *parts[9]) {
        char *endptr = NULL;
        double val = strtod(parts[9], &endptr);
        if (endptr != parts[9] && *endptr == '\0') {
            af->angle = (float)val;
        } else {
            af->angle = 0.0f; /* default value */
        }
    }
    
    return af;
}

/* === Animation Structure === */

struct ikm_animation {
    ikm_sff_t* sff;
    ikm_palette_list_t* palette_data;
    ikm_sprite_t* spr;
    ikm_anim_frame_t** frames;
    size_t frame_count;
    int32_t loopstart;
    int32_t curtime;
    int32_t curelem;
    int32_t curelemtime;
    int32_t drawidx;
    int32_t totaltime;
    int32_t looptime;
    int32_t prelooptime;
    int16_t mask;
    ikm_trans_type_t trans_type;
    int16_t src_alpha;
    int16_t dst_alpha;
    bool newframe;
    bool loopend;
    float scale_x;
    float scale_y;
    float angle;
    ikm_trans_type_t cur_trans;
    float start_scale[2];
};

ikm_animation_t* ikm_animation_create(ikm_sff_t* sff, ikm_palette_list_t* pal) {
    ikm_animation_t* anim = calloc(1, sizeof(ikm_animation_t));
    if (!anim) return NULL;
    
    anim->sff = sff;
    anim->palette_data = pal;
    anim->mask = -1;
    anim->trans_type = IKM_TT_DEFAULT;
    anim->src_alpha = -1;
    anim->dst_alpha = 255;
    anim->scale_x = 1.0f;
    anim->scale_y = 1.0f;
    anim->start_scale[0] = 1.0f;
    anim->start_scale[1] = 1.0f;
    
    return anim;
}

void ikm_animation_destroy(ikm_animation_t* anim) {
    if (!anim) return;
    
    for (size_t i = 0; i < anim->frame_count; i++) {
        ikm_anim_frame_destroy(anim->frames[i]);
    }
    free(anim->frames);
    free(anim);
}

/* Add frame to animation */
int ikm_animation_add_frame(ikm_animation_t* anim, ikm_anim_frame_t* frame) {
    if (!anim || !frame) return -1;
    
    ikm_anim_frame_t** new_frames = realloc(anim->frames,
        (anim->frame_count + 1) * sizeof(ikm_anim_frame_t*));
    if (!new_frames) return -1;
    
    new_frames[anim->frame_count] = frame;
    anim->frames = new_frames;
    anim->frame_count++;
    
    return 0;
}

/* Reset animation to start */
void ikm_animation_reset(ikm_animation_t* anim) {
    if (!anim) return;
    
    anim->curtime = 0;
    anim->curelem = 0;
    anim->curelemtime = 0;
    anim->drawidx = 0;
    anim->newframe = true;
    anim->loopend = false;
    
    if (anim->frame_count > 0) {
        anim->spr = ikm_sff_get_sprite(anim->sff, 
                                       anim->frames[0]->group,
                                       anim->frames[0]->number);
    }
}

/* Update animation (call each frame) */
void ikm_animation_update(ikm_animation_t* anim) {
    if (!anim || anim->frame_count == 0) return;
    
    anim->newframe = false;
    
    if (anim->curelem >= (int32_t)anim->frame_count) {
        anim->loopend = true;
        return;
    }
    
    ikm_anim_frame_t* cur = anim->frames[anim->curelem];
    
    /* Check if we should advance to next frame */
    if (cur->time >= 0 && anim->curelemtime >= cur->time) {
        anim->curelem++;
        anim->curelemtime = 0;
        anim->newframe = true;
        
        if (anim->curelem >= (int32_t)anim->frame_count) {
            /* Loop back to loop start */
            if (anim->loopstart >= 0 && anim->loopstart < (int32_t)anim->frame_count) {
                anim->curelem = anim->loopstart;
            } else {
                anim->curelem = 0;
            }
        }
        
        /* Load new sprite */
        if (anim->curelem < (int32_t)anim->frame_count) {
            cur = anim->frames[anim->curelem];
            anim->spr = ikm_sff_get_sprite(anim->sff, cur->group, cur->number);
        }
    }
    
    anim->curelemtime++;
    anim->curtime++;
}

/* Get current sprite */
ikm_sprite_t* ikm_animation_get_sprite(const ikm_animation_t* anim) {
    return anim ? anim->spr : NULL;
}

/* Get current frame data */
const ikm_anim_frame_t* ikm_animation_get_frame(const ikm_animation_t* anim) {
    if (!anim || anim->curelem >= (int32_t)anim->frame_count) return NULL;
    return anim->frames[anim->curelem];
}

/* Check if animation has looped */
bool ikm_animation_has_looped(const ikm_animation_t* anim) {
    return anim ? anim->loopend : false;
}

/* === AIR File Loading === */

typedef struct {
    int32_t action_no;
    ikm_animation_t* animation;
} ikm_air_action_t;

typedef struct {
    ikm_air_action_t** actions;
    size_t action_count;
    ikm_sff_t* sff;
    ikm_palette_list_t* palette_list;
} ikm_air_file_t;

ikm_air_file_t* ikm_air_create(ikm_sff_t* sff, ikm_palette_list_t* pal) {
    ikm_air_file_t* air = calloc(1, sizeof(ikm_air_file_t));
    if (!air) return NULL;
    
    air->sff = sff;
    air->palette_list = pal;
    
    return air;
}

void ikm_air_destroy(ikm_air_file_t* air) {
    if (!air) return;
    
    for (size_t i = 0; i < air->action_count; i++) {
        ikm_animation_destroy(air->actions[i]->animation);
        free(air->actions[i]);
    }
    free(air->actions);
    free(air);
}

/* Load AIR file */
ikm_air_file_t* ikm_air_load(const char* filename, ikm_sff_t* sff, ikm_palette_list_t* pal) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Failed to open AIR file: %s\n", filename);
        return NULL;
    }
    
    ikm_air_file_t* air = ikm_air_create(sff, pal);
    if (!air) {
        fclose(f);
        return NULL;
    }
    
    char line[1024];
    ikm_animation_t* current_anim = NULL;
    int32_t current_action = -1;
    
    while (fgets(line, sizeof(line), f)) {
        /* Trim whitespace */
        char* start = line;
        while (*start && isspace((unsigned char)*start)) start++;
        if (!*start || *start == ';') continue; /* Empty or comment */
        
        char* end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) *end-- = '\0';
        
        /* Check for [Begin Action N] */
        if (strncmp(start, "[Begin Action ", 14) == 0) {
            char* num_start = start + 14;
            current_action = atoi(num_start);
            
            /* Create new animation for this action */
            current_anim = ikm_animation_create(sff, pal);
            if (current_anim) {
                /* Add to actions list */
                ikm_air_action_t* action = calloc(1, sizeof(ikm_air_action_t));
                if (action) {
                    action->action_no = current_action;
                    action->animation = current_anim;
                    
                    ikm_air_action_t** new_actions = realloc(air->actions,
                        (air->action_count + 1) * sizeof(ikm_air_action_t*));
                    if (new_actions) {
                        new_actions[air->action_count] = action;
                        air->actions = new_actions;
                        air->action_count++;
                    }
                }
            }
            continue;
        }
        
        /* Check for Loopstart */
        if (current_anim && strncmp(start, "Loopstart", 9) == 0) {
            current_anim->loopstart = (int32_t)current_anim->frame_count;
            continue;
        }
        
        /* Check for animation frame line */
        if (current_anim && (isdigit((unsigned char)start[0]) || start[0] == '-')) {
            ikm_anim_frame_t* frame = ikm_anim_frame_parse(start);
            if (frame) {
                ikm_animation_add_frame(current_anim, frame);
            }
        }
    }
    
    fclose(f);
    
    printf("[INFO] Loaded AIR: %s (%zu actions)\n", filename, air->action_count);
    
    return air;
}

/* Get animation for action number */
ikm_animation_t* ikm_air_get_animation(ikm_air_file_t* air, int32_t action_no) {
    if (!air) return NULL;
    
    for (size_t i = 0; i < air->action_count; i++) {
        if (air->actions[i]->action_no == action_no) {
            return air->actions[i]->animation;
        }
    }
    
    return NULL;
}
