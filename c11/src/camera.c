/* camera.c - Camera system implementation
 * C11 analog of src/camera.go (547 lines)
 * Handles stage camera movement, zoom, bounds, and tracking
 */

#include "ikemen_engine.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

/* Camera view modes */
typedef enum {
    IKM_CAMERA_FIGHTING = 0,
    IKM_CAMERA_FOLLOW = 1,
    IKM_CAMERA_FREE = 2
} ikm_camera_view_t;

/* Stage camera configuration */
typedef struct {
    int32_t startx, starty;
    int32_t boundleft, boundright, boundhigh, boundlow;
    float verticalfollow;
    int32_t floortension, tensionhigh, tensionlow;
    bool lowestcap;
    int32_t tension;
    float tensionvel;
    int32_t overdrawhigh, overdrawlow;
    int32_t cuthigh, cutlow;
    int32_t localcoord[2];
    float localscl;
    int32_t zoffset;
    float ztopscale, zbotscale, depthtoscreen;
    float topz, botz;
    float startzoom, zoomin, zoomout;
    bool ytensionenable, autocenter, zoomanchor;
    float boundhighzoomdelta, verticalfollowzoomdelta;
    float zoomindelay, zoomindelaytime, zoominspeed, zoomoutspeed, yscrollspeed;
    float fov, yshift, far, near;
    float aspectcorrection, zoomanchorcorrection;
    float ywithoutbound;
    float highest, prevHighest, lowest, prevLowest;
    float leftest, prevLeftest, rightest, prevRightest;
    float leftestvel, rightestvel;
    bool roundstart;
    float maxRight, minLeft;
} ikm_stage_camera_t;

/* Camera state */
struct ikm_camera {
    ikm_stage_camera_t stage_cam;
    ikm_camera_view_t view;
    bool zoom_enable;
    float zoomdelay;
    float pos[2], screen_pos[2], offset[2];
    float xmin, xmax;
    float scale, min_scale;
    float boundL, boundR, boundH, boundLo;
    float zoff, half_width;
    ikm_char_t* follow_char;
};

ikm_camera_t* ikm_camera_create(void) {
    ikm_camera_t* c = calloc(1, sizeof(ikm_camera_t));
    if (!c) return NULL;
    
    /* Initialize stage camera defaults */
    c->stage_cam.verticalfollow = 0.2f;
    c->stage_cam.tensionvel = 1.0f;
    c->stage_cam.tension = 50;
    c->stage_cam.cuthigh = 0;
    c->stage_cam.cutlow = INT32_MIN;
    c->stage_cam.localcoord[0] = 320;
    c->stage_cam.localcoord[1] = 240;
    c->stage_cam.localscl = 1.0f;  /* sys.gameWidth / 320 */
    c->stage_cam.topz = 0.0f;
    c->stage_cam.botz = 0.0f;
    c->stage_cam.ztopscale = 1.0f;
    c->stage_cam.zbotscale = 1.0f;
    c->stage_cam.depthtoscreen = 1.0f;
    c->stage_cam.startzoom = 1.0f;
    c->stage_cam.zoomin = 1.0f;
    c->stage_cam.zoomout = 1.0f;
    c->stage_cam.ytensionenable = false;
    c->stage_cam.fov = 40.0f;
    c->stage_cam.yshift = 0.0f;
    c->stage_cam.far = 10000.0f;
    c->stage_cam.near = 0.1f;
    c->stage_cam.zoomindelay = 0.0f;
    c->stage_cam.zoominspeed = 1.0f;
    c->stage_cam.zoomoutspeed = 1.0f;
    c->stage_cam.yscrollspeed = 1.0f;
    
    c->view = IKM_CAMERA_FIGHTING;
    
    return c;
}

void ikm_camera_destroy(ikm_camera_t* c) {
    free(c);
}

float ikm_camera_base_scale(ikm_camera_t* c) {
    return c ? c->stage_cam.ztopscale : 1.0f;
}

float ikm_camera_ground_level(ikm_camera_t* c) {
    if (!c) return 0.0f;
    return c->zoff - c->stage_cam.aspectcorrection - c->stage_cam.zoomanchorcorrection;
}

void ikm_camera_reset(ikm_camera_t* c) {
    if (!c) return;
    
    ikm_stage_camera_t* sc = &c->stage_cam;
    
    /* Calculate zoom enable - branchless */
    int zoom_check = (sc->zoomin != 1.0f) | (sc->zoomout != 1.0f);
    c->zoom_enable = zoom_check != 0;  /* Depends on config */
    
    /* Calculate bounds */
    float zoom_factor = ((1.0f - sc->zoomout) * 100.0f * sc->zoomout) * 
                        (1.0f / sc->zoomout) * (1.0f / sc->zoomout) * 1.6f * 1.0f;  /* gameWidth/320 */
    c->boundL = (float)(sc->boundleft - sc->startx) * sc->localscl - zoom_factor;
    c->boundR = (float)(sc->boundright - sc->startx) * sc->localscl + zoom_factor;
    
    c->half_width = 320.0f;  /* gameWidth / 2 */
    c->xmin = c->boundL - c->half_width / ikm_camera_base_scale(c);
    c->xmax = c->boundR + c->half_width / ikm_camera_base_scale(c);
    
    sc->aspectcorrection = 0.0f;
    sc->zoomanchorcorrection = 0.0f;
    
    /* Branchless max */
    sc->zoomin = (sc->zoomin > sc->zoomout) ? sc->zoomin : sc->zoomout;
    
    /* Handle cutlow default */
    if (sc->cutlow == INT32_MIN) {
        sc->cutlow = (int32_t)((float)(sc->localcoord[1] - sc->zoffset) - 
                               (float)sc->localcoord[1] * 0.05f);
    }
    
    /* Calculate bounds */
    c->boundH = (float)sc->boundhigh * sc->localscl;
    c->boundLo = (float)(sc->boundhigh > sc->boundlow ? sc->boundhigh : sc->boundlow) * sc->localscl;
    sc->boundlow = sc->boundhigh > sc->boundlow ? sc->boundhigh : sc->boundlow;
    
    /* Clamp tensionvel - branchless */
    sc->tensionvel = (sc->tensionvel < 0.0f) ? 0.0f : 
                     ((sc->tensionvel > 20.0f) ? 20.0f : sc->tensionvel);
    
    /* Check vertical tension */
    if (sc->verticalfollow < 0.0f) {
        sc->ytensionenable = true;
    }
    
    /* Calculate min scale */
    float xminscl = 640.0f / (640.0f - c->boundL + c->boundR);  /* gameWidth */
    c->min_scale = (sc->zoomout > xminscl) ? sc->zoomout : 
                   ((sc->zoomin < xminscl) ? sc->zoomin : xminscl);
    
    sc->maxRight = (float)sc->boundright * sc->localscl + c->half_width / sc->zoomout;
    sc->minLeft = (float)sc->boundleft * sc->localscl - c->half_width / sc->zoomout;
}

void ikm_camera_init(ikm_camera_t* c) {
    if (!c) return;
    
    ikm_camera_reset(c);
    c->view = IKM_CAMERA_FIGHTING;
    c->stage_cam.roundstart = true;
    c->scale = c->stage_cam.startzoom;
    
    ikm_stage_camera_t* sc = &c->stage_cam;
    c->pos[0] = (float)sc->startx * sc->localscl;
    c->pos[1] = (float)sc->starty * sc->localscl;
    sc->ywithoutbound = (float)sc->starty * sc->localscl;
    
    sc->zoomindelaytime = sc->zoomindelay;
}

void ikm_camera_reset_tracking(ikm_camera_t* c) {
    if (!c) return;
    
    ikm_stage_camera_t* sc = &c->stage_cam;
    sc->leftest = FLT_MAX;
    sc->rightest = -FLT_MAX;
    sc->highest = FLT_MAX;
    sc->lowest = -FLT_MAX;
    sc->leftestvel = 0.0f;
    sc->rightestvel = 0.0f;
}

void ikm_camera_save_restore_tracking(ikm_camera_t* c) {
    if (!c) return;
    
    ikm_stage_camera_t* sc = &c->stage_cam;
    
    /* Branchless save/restore using conditional move pattern */
    float is_invalid_high = (sc->highest == FLT_MAX);
    sc->highest = is_invalid_high ? sc->prevHighest : sc->highest;
    sc->prevHighest = is_invalid_high ? sc->prevHighest : sc->highest;
    
    float is_invalid_low = (sc->lowest == -FLT_MAX);
    sc->lowest = is_invalid_low ? sc->prevLowest : sc->lowest;
    sc->prevLowest = is_invalid_low ? sc->prevLowest : sc->lowest;
    
    float is_invalid_left = (sc->leftest == FLT_MAX);
    sc->leftest = is_invalid_left ? sc->prevLeftest : sc->leftest;
    sc->prevLeftest = is_invalid_left ? sc->prevLeftest : sc->leftest;
    
    float is_invalid_right = (sc->rightest == -FLT_MAX);
    sc->rightest = is_invalid_right ? sc->prevRightest : sc->rightest;
    sc->prevRightest = is_invalid_right ? sc->prevRightest : sc->rightest;
}

void ikm_camera_update(ikm_camera_t* c, float scl, float x, float y) {
    if (!c) return;
    
    c->scale = ikm_camera_base_scale(c) * scl;
    c->zoff = (float)c->stage_cam.zoffset * c->stage_cam.localscl;
    
    /* Update offsets */
    c->offset[0] = 0.0f;  /* Would be stage.bga.offset[0] * stage.localscl * scl */
    c->offset[1] = 0.0f;
    
    c->screen_pos[0] = x - c->half_width / c->scale - c->offset[0];
    c->screen_pos[1] = y - (ikm_camera_ground_level(c) - 480.0f * scl) / c->scale - c->offset[1];
    c->pos[0] = x;
    c->pos[1] = y;
}

float ikm_camera_scale_bound(ikm_camera_t* c, float scl, float sclmul) {
    if (!c || !c->zoom_enable) return 1.0f;
    
    ikm_stage_camera_t* sc = &c->stage_cam;
    float result = scl * sclmul;
    
    /* Clamp to [min_scale, zoomin] */
    result = (result < c->min_scale) ? c->min_scale : result;
    result = (result > sc->zoomin) ? sc->zoomin : result;
    
    return result;
}

float ikm_camera_x_bound(ikm_camera_t* c, float scl, float x) {
    if (!c) return x;
    
    float min = c->boundL - c->half_width + c->half_width / scl;
    float max = c->boundR + c->half_width - c->half_width / scl;
    
    /* Branchless clamp */
    x = (x < min) ? min : x;
    x = (x > max) ? max : x;
    
    return x;
}

void ikm_camera_action(ikm_camera_t* c, float x, float y, float scale, bool pause,
                       float* new_x, float* new_y, float* new_scale) {
    if (!c || !new_x || !new_y || !new_scale) return;
    
    *new_x = x;
    *new_y = y;
    *new_scale = scale;
    
    /* Simplified - full implementation requires character tracking data */
    switch (c->view) {
        case IKM_CAMERA_FIGHTING:
            ikm_camera_save_restore_tracking(c);
            /* Complex fighting camera logic would go here */
            break;
            
        case IKM_CAMERA_FOLLOW:
            if (c->follow_char) {
                float char_pos[3];
                ikm_char_get_position(c->follow_char, &char_pos[0], &char_pos[1], &char_pos[2]);
                *new_x = char_pos[0];
                *new_y = char_pos[1] * powf(c->stage_cam.verticalfollow, 
                                             1.0f < 1.0f / powf(c->scale, 4.0f) ? 1.0f : 
                                             1.0f / powf(c->scale, 4.0f));
                *new_scale = 1.0f;
            }
            break;
            
        case IKM_CAMERA_FREE:
            *new_x = c->pos[0];
            *new_y = c->pos[1];
            c->stage_cam.ywithoutbound = *new_y;
            *new_scale = 1.0f;
            break;
    }
    
    c->stage_cam.roundstart = false;
}

void ikm_camera_reset_zoomdelay(ikm_camera_t* c) {
    if (c) c->zoomdelay = 0.0f;
}
