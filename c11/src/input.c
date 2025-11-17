/*
 * Ikemen C11 Engine - Input System
 * Implements input handling matching Go's input.go and input_glfw.go
 */

#include "ikemen_engine.h"
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <string.h>

#define IKM_MAX_KEYS 512

struct ikm_input {
    GLFWwindow* window;
    int key_state[IKM_MAX_KEYS];
    int prev_key_state[IKM_MAX_KEYS];
    double mouse_x;
    double mouse_y;
    int mouse_buttons[8];
};

/* Key mapping from GLFW to engine keys */
__attribute__((unused)) static int glfw_to_engine_key(int glfw_key) {
    /* Direct mapping for now - TODO: implement full mapping table */
    return glfw_key;
}

ikm_input_t* ikm_input_create(ikm_window_t* window) {
    ikm_input_t* input = (ikm_input_t*)calloc(1, sizeof(ikm_input_t));
    if (!input) {
        ikm_set_error("Memory allocation failed");
        return NULL;
    }
    
    input->window = ikm_window_get_handle(window);
    if (!input->window) {
        ikm_set_error("Invalid window handle");
        free(input);
        return NULL;
    }
    
    /* Initialize key states */
    memset(input->key_state, 0, sizeof(input->key_state));
    memset(input->prev_key_state, 0, sizeof(input->prev_key_state));
    memset(input->mouse_buttons, 0, sizeof(input->mouse_buttons));
    
    ikm_log(IKM_LOG_INFO, "Input system initialized");
    
    return input;
}

void ikm_input_destroy(ikm_input_t* input) {
    if (input) {
        free(input);
    }
}

void ikm_input_update(ikm_input_t* input) {
    if (!input) return;
    
    /* Save previous frame state */
    memcpy(input->prev_key_state, input->key_state, sizeof(input->key_state));
    
    /* Update keyboard state */
    for (int i = 0; i < IKM_MAX_KEYS; i++) {
        int state = glfwGetKey(input->window, i);
        input->key_state[i] = (state == GLFW_PRESS || state == GLFW_REPEAT);
    }
    
    /* Update mouse state */
    glfwGetCursorPos(input->window, &input->mouse_x, &input->mouse_y);
    for (int i = 0; i < 8; i++) {
        input->mouse_buttons[i] = glfwGetMouseButton(input->window, i) == GLFW_PRESS;
    }
}

int ikm_input_key_pressed(ikm_input_t* input, int key) {
    if (!input || key < 0 || key >= IKM_MAX_KEYS) return 0;
    return input->key_state[key] && !input->prev_key_state[key];
}

int ikm_input_key_released(ikm_input_t* input, int key) {
    if (!input || key < 0 || key >= IKM_MAX_KEYS) return 0;
    return !input->key_state[key] && input->prev_key_state[key];
}

int ikm_input_key_down(ikm_input_t* input, int key) {
    if (!input || key < 0 || key >= IKM_MAX_KEYS) return 0;
    return input->key_state[key];
}

void ikm_input_get_mouse_position(ikm_input_t* input, double* x, double* y) {
    if (input) {
        if (x) *x = input->mouse_x;
        if (y) *y = input->mouse_y;
    }
}

int ikm_input_mouse_button_pressed(ikm_input_t* input, int button) {
    if (!input || button < 0 || button >= 8) return 0;
    return input->mouse_buttons[button];
}

/* Command state checking (matching Go's commandGetState) */
int ikm_input_command_state(ikm_input_t* input, const char* command) {
    if (!input || !command) return 0;
    
    /* Simple command checking - TODO: implement full command matching */
    if (strcmp(command, "up") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_UP);
    } else if (strcmp(command, "down") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_DOWN);
    } else if (strcmp(command, "left") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_LEFT);
    } else if (strcmp(command, "right") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_RIGHT);
    } else if (strcmp(command, "a") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_A);
    } else if (strcmp(command, "b") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_S);
    } else if (strcmp(command, "c") == 0) {
        return ikm_input_key_down(input, GLFW_KEY_D);
    }
    
    return 0;
}
