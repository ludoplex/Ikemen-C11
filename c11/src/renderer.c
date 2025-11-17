/*
 * Ikemen C11 Engine - OpenGL Renderer
 * Implements OpenGL 3.2 rendering matching Go's render_gl_gl32.go
 */

#define UNUSED(x) (void)(x)
#include "ikemen_engine.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct ikm_renderer {
    int width;
    int height;
    float clear_r, clear_g, clear_b, clear_a;
    int initialized;
};

ikm_renderer_t* ikm_renderer_create(void) {
    ikm_renderer_t* renderer = (ikm_renderer_t*)calloc(1, sizeof(ikm_renderer_t));
    if (!renderer) {
        ikm_set_error("Memory allocation failed");
        return NULL;
    }
    
    renderer->width = 640;
    renderer->height = 480;
    renderer->clear_r = 0.0f;
    renderer->clear_g = 0.0f;
    renderer->clear_b = 0.0f;
    renderer->clear_a = 1.0f;
    
    return renderer;
}

void ikm_renderer_destroy(ikm_renderer_t* renderer) {
    if (renderer) {
        free(renderer);
    }
}

int ikm_renderer_initialize(ikm_renderer_t* renderer, int width, int height) {
    if (!renderer) {
        ikm_set_error("Invalid renderer");
        return IKM_ERROR_UNKNOWN;
    }
    
    renderer->width = width;
    renderer->height = height;
    
    /* Set OpenGL state */
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    renderer->initialized = 1;
    ikm_log(IKM_LOG_INFO, "OpenGL renderer initialized: %dx%d", width, height);
    
    /* Log OpenGL info */
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer_name = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);
    ikm_log(IKM_LOG_INFO, "OpenGL: %s / %s / %s", vendor, renderer_name, version);
    
    return IKM_SUCCESS;
}

void ikm_renderer_begin_frame(ikm_renderer_t* renderer) {
    if (!renderer) return;
    
    glClearColor(renderer->clear_r, renderer->clear_g, renderer->clear_b, renderer->clear_a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ikm_renderer_end_frame(ikm_renderer_t* renderer) {
    if (!renderer) return;
    /* Frame complete - actual buffer swap happens in window */
}

void ikm_renderer_set_clear_color(ikm_renderer_t* renderer, float r, float g, float b, float a) {
    if (renderer) {
        renderer->clear_r = r;
        renderer->clear_g = g;
        renderer->clear_b = b;
        renderer->clear_a = a;
    }
}

void ikm_renderer_set_viewport(ikm_renderer_t* renderer, int x, int y, int width, int height) {
    if (renderer) {
        glViewport(x, y, width, height);
        renderer->width = width;
        renderer->height = height;
    }
}

/* Simple quad rendering for testing */
void ikm_renderer_draw_quad(ikm_renderer_t* renderer, float x, float y, float w, float h, 
                            float r, float g, float b, float a) {
    if (!renderer) return;
    
    /* Convert screen coordinates to normalized device coordinates */
    float x1 = (x / renderer->width) * 2.0f - 1.0f;
    float y1 = 1.0f - (y / renderer->height) * 2.0f;
    float x2 = ((x + w) / renderer->width) * 2.0f - 1.0f;
    float y2 = 1.0f - ((y + h) / renderer->height) * 2.0f;
    
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

const char* ikm_renderer_get_name(ikm_renderer_t* renderer) {
    UNUSED(renderer);
    return "OpenGL 3.2";
}
