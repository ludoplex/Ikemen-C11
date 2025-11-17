/*
 * Ikemen C11 Engine - Main Entry Point
 * 
 * This executable demonstrates the C11 engine bootstrap and performs
 * asset compatibility checks against the directory structure used by
 * the Go reference implementation.
 * 
 * Usage: ikemen_c11 [base_path]
 *   base_path: Optional path to the engine root directory (defaults to current dir)
 * 
 * Exit codes:
 *   0 - Success: Asset compatibility check passed
 *   1 - Error: Asset compatibility check failed or invalid arguments
 */

#include "ikemen_engine.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* base_path = NULL;
    int result;
    
    /* Print engine information */
    printf("Ikemen C11 Engine\n");
    printf("Version: %s\n", ikm_engine_version_string());
    printf("Build: C11 Standard (ISO/IEC 9899:2011)\n");
    printf("\n");
    
    /* Parse command line arguments */
    if (argc > 1) {
        base_path = argv[1];
        printf("Using base path: %s\n", base_path);
    } else {
        printf("Using base path: . (current directory)\n");
    }
    printf("\n");
    
    /* Perform asset compatibility check */
    printf("Performing asset compatibility check...\n");
    printf("Checking for required directories:\n");
    printf("  - data/      (Common data files)\n");
    printf("  - external/  (External resources)\n");
    printf("  - font/      (Font files)\n");
    printf("\n");
    
    result = ikm_check_asset_compatibility(base_path);
    
    if (result == IKM_SUCCESS) {
        printf("✓ Asset compatibility check PASSED\n");
        printf("All required directories found and accessible.\n");
        printf("\n");
        printf("Details: %s\n", ikm_get_last_error());
        return EXIT_SUCCESS;
    } else {
        printf("✗ Asset compatibility check FAILED\n");
        printf("\n");
        
        /* Print specific error information */
        switch (result) {
            case IKM_ERROR_FILE_NOT_FOUND:
                printf("Error type: Missing required directory\n");
                break;
            case IKM_ERROR_INVALID_PATH:
                printf("Error type: Invalid path\n");
                break;
            case IKM_ERROR_PERMISSION_DENIED:
                printf("Error type: Permission denied\n");
                break;
            default:
                printf("Error type: Unknown error (code %d)\n", result);
                break;
        }
        
        printf("Details: %s\n", ikm_get_last_error());
        printf("\n");
        printf("The C11 engine requires the same asset directory structure\n");
        printf("as the Go reference implementation. Please ensure all required\n");
        printf("directories exist in the specified base path.\n");
        
        return EXIT_FAILURE;
    }
}
