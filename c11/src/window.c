/*
 * Ikemen C11 Engine - Window Management
 * Implements GLFW window creation matching Go's system_glfw.go
 */

#include "ikemen_engine.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ikm_window {
    GLFWwindow* handle;
    int width;
    int height;
    int fullscreen;
    char title[256];
};

static void error_callback(int error, const char* description) {
    ikm_log(IKM_LOG_ERROR, "GLFW Error %d: %s", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    UNUSED(scancode); UNUSED(mods);
    /* Input handling - TODO: integrate with input system */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

ikm_window_t* ikm_window_create(const char* title, int width, int height, int fullscreen) {
    ikm_window_t* window = (ikm_window_t*)calloc(1, sizeof(ikm_window_t));
    if (!window) {
        ikm_set_error("Memory allocation failed");
        return NULL;
    }
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        ikm_set_error("Failed to initialize GLFW");
        free(window);
        return NULL;
    }
    
    /* Set OpenGL version hints (matching Go's OpenGL 3.2 core) */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
    window->handle = glfwCreateWindow(width, height, title, monitor, NULL);
    
    if (!window->handle) {
        ikm_set_error("Failed to create GLFW window");
        glfwTerminate();
        free(window);
        return NULL;
    }
    
    window->width = width;
    window->height = height;
    window->fullscreen = fullscreen;
    strncpy(window->title, title, sizeof(window->title) - 1);
    
    glfwSetKeyCallback(window->handle, key_callback);
    glfwMakeContextCurrent(window->handle);
    glfwSwapInterval(1); /* Enable vsync */
    
    ikm_log(IKM_LOG_INFO, "Window created: %dx%d %s", 
            width, height, fullscreen ? "(fullscreen)" : "(windowed)");
    
    return window;
}

void ikm_window_destroy(ikm_window_t* window) {
    if (window) {
        if (window->handle) {
            glfwDestroyWindow(window->handle);
        }
        glfwTerminate();
        free(window);
    }
}

int ikm_window_should_close(ikm_window_t* window) {
    return window ? glfwWindowShouldClose(window->handle) : 1;
}

void ikm_window_poll_events(ikm_window_t* window) {
    if (window) {
        glfwPollEvents();
    }
}

void ikm_window_swap_buffers(ikm_window_t* window) {
    if (window && window->handle) {
        glfwSwapBuffers(window->handle);
    }
}

void ikm_window_get_size(ikm_window_t* window, int* width, int* height) {
    if (window && window->handle) {
        glfwGetWindowSize(window->handle, width, height);
    }
}

void ikm_window_set_title(ikm_window_t* window, const char* title) {
    if (window && window->handle) {
        glfwSetWindowTitle(window->handle, title);
        strncpy(window->title, title, sizeof(window->title) - 1);
    }
}

void* ikm_window_get_handle(ikm_window_t* window) {
    return window ? window->handle : NULL;
}
