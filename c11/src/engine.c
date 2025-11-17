/*
 * Ikemen C11 Engine - Core Implementation
 * 
 * This file implements the core functionality of the Ikemen C11 engine,
 * focusing on version reporting and asset compatibility checks.
 * 
 * Implementation notes:
 * - Uses only C11 standard library and POSIX APIs
 * - Asset paths match those used by the Go reference implementation
 * - Designed for cross-platform compatibility (Linux, Windows, macOS)
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

/* Maximum length for error messages */
#define MAX_ERROR_LENGTH 512

/* Thread-local error message buffer */
static char last_error_msg[MAX_ERROR_LENGTH] = "";

/* Version string buffer */
static char version_string_buf[32];

/*
 * Internal helper: Check if a directory exists and is accessible.
 * 
 * Returns: 1 if directory exists and is accessible, 0 otherwise.
 * On error, updates last_error_msg with details.
 */
static int check_directory_exists(const char* path) {
    struct stat st;
    
    if (stat(path, &st) != 0) {
        if (errno == ENOENT) {
            snprintf(last_error_msg, MAX_ERROR_LENGTH,
                    "Directory not found: %s", path);
            return 0;
        } else if (errno == EACCES) {
            snprintf(last_error_msg, MAX_ERROR_LENGTH,
                    "Permission denied accessing: %s", path);
            return 0;
        } else {
            snprintf(last_error_msg, MAX_ERROR_LENGTH,
                    "Error accessing %s: %s", path, strerror(errno));
            return 0;
        }
    }
    
    /* Check if it's actually a directory */
    if (!S_ISDIR(st.st_mode)) {
        snprintf(last_error_msg, MAX_ERROR_LENGTH,
                "Path exists but is not a directory: %s", path);
        return 0;
    }
    
    return 1;
}

/*
 * Internal helper: Build a path by concatenating base and relative paths.
 * 
 * Parameters:
 *   buffer - Output buffer for the constructed path
 *   buffer_size - Size of the output buffer
 *   base - Base path (e.g., ".")
 *   relative - Relative path to append (e.g., "data")
 * 
 * Returns: 1 on success, 0 if buffer too small
 */
static int build_path(char* buffer, size_t buffer_size,
                     const char* base, const char* relative) {
    /* Handle current directory special case */
    if (strcmp(base, ".") == 0) {
        if (snprintf(buffer, buffer_size, "%s", relative) >= (int)buffer_size) {
            return 0;
        }
    } else {
        /* Construct base/relative path */
        if (snprintf(buffer, buffer_size, "%s/%s", base, relative) >= (int)buffer_size) {
            return 0;
        }
    }
    return 1;
}

/* Version query functions */

int ikm_engine_major_version(void) {
    return IKM_ENGINE_MAJOR;
}

int ikm_engine_minor_version(void) {
    return IKM_ENGINE_MINOR;
}

int ikm_engine_patch_version(void) {
    return IKM_ENGINE_PATCH;
}

const char* ikm_engine_version_string(void) {
    snprintf(version_string_buf, sizeof(version_string_buf),
            "%d.%d.%d", IKM_ENGINE_MAJOR, IKM_ENGINE_MINOR, IKM_ENGINE_PATCH);
    return version_string_buf;
}

/* Asset compatibility check implementation */

int ikm_check_asset_compatibility(const char* base_path) {
    char path_buffer[512];
    const char* effective_base;
    
    /* Clear previous error */
    last_error_msg[0] = '\0';
    
    /* Use current directory if base_path is NULL */
    effective_base = (base_path == NULL) ? "." : base_path;
    
    /* Check if base path itself is accessible */
    if (!check_directory_exists(effective_base)) {
        return IKM_ERROR_INVALID_PATH;
    }
    
    /*
     * Check required directories in order of importance.
     * These match the directory structure expected by the Go implementation.
     * See Go source references:
     * - src/char.go: references "data/" path prefix
     * - src/stage.go: references "data/" in search paths
     * - BUILDING.md: lists data, external, font as required assets
     */
    
    /* Check data/ directory */
    if (!build_path(path_buffer, sizeof(path_buffer), effective_base, "data")) {
        snprintf(last_error_msg, MAX_ERROR_LENGTH, "Path buffer too small");
        return IKM_ERROR_INVALID_PATH;
    }
    if (!check_directory_exists(path_buffer)) {
        /* last_error_msg already set by check_directory_exists */
        if (errno == EACCES) {
            return IKM_ERROR_PERMISSION_DENIED;
        }
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    /* Check external/ directory */
    if (!build_path(path_buffer, sizeof(path_buffer), effective_base, "external")) {
        snprintf(last_error_msg, MAX_ERROR_LENGTH, "Path buffer too small");
        return IKM_ERROR_INVALID_PATH;
    }
    if (!check_directory_exists(path_buffer)) {
        if (errno == EACCES) {
            return IKM_ERROR_PERMISSION_DENIED;
        }
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    /* Check font/ directory */
    if (!build_path(path_buffer, sizeof(path_buffer), effective_base, "font")) {
        snprintf(last_error_msg, MAX_ERROR_LENGTH, "Path buffer too small");
        return IKM_ERROR_INVALID_PATH;
    }
    if (!check_directory_exists(path_buffer)) {
        if (errno == EACCES) {
            return IKM_ERROR_PERMISSION_DENIED;
        }
        return IKM_ERROR_FILE_NOT_FOUND;
    }
    
    /* All checks passed */
    snprintf(last_error_msg, MAX_ERROR_LENGTH, "Asset compatibility check passed");
    return IKM_SUCCESS;
}

const char* ikm_get_last_error(void) {
    return last_error_msg;
}
