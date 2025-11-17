cmake_minimum_required(VERSION 3.20)

# Project definition
project(IkemenC11
    VERSION 0.1.0
    DESCRIPTION "Ikemen fighting game engine - C11 refactor"
    LANGUAGES C
)

# C11 standard required
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler warnings and flags
if(MSVC)
    # MSVC specific flags
    add_compile_options(
        /W4          # Warning level 4
        /WX          # Treat warnings as errors
        /permissive- # Standards conformance
    )
else()
    # GCC/Clang flags
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wformat=2
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Wold-style-definition
        -Wno-unused-parameter
    )
endif()

# Build type specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    add_compile_definitions(DEBUG)
    
    # Enable sanitizers in debug mode on non-Windows
    if(NOT MSVC)
        option(ENABLE_ASAN "Enable AddressSanitizer" ON)
        option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)
        
        if(ENABLE_ASAN)
            message(STATUS "AddressSanitizer enabled")
            add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
            add_link_options(-fsanitize=address)
        endif()
        
        if(ENABLE_UBSAN)
            message(STATUS "UndefinedBehaviorSanitizer enabled")
            add_compile_options(-fsanitize=undefined)
            add_link_options(-fsanitize=undefined)
        endif()
    endif()
else()
    message(STATUS "Building in Release mode")
    add_compile_definitions(NDEBUG)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)

# Testing
enable_testing()
add_subdirectory(tests)

# Print configuration summary
message(STATUS "==============================================")
message(STATUS "  Ikemen C11 Configuration Summary")
message(STATUS "==============================================")
message(STATUS "Version:       ${PROJECT_VERSION}")
message(STATUS "Build type:    ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler:    ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "C Standard:    C${CMAKE_C_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
if(NOT MSVC)
    if(ENABLE_ASAN)
        message(STATUS "AddressSanitizer: ON")
    else()
        message(STATUS "AddressSanitizer: OFF")
    endif()
    if(ENABLE_UBSAN)
        message(STATUS "UndefinedBehaviorSanitizer: ON")
    else()
        message(STATUS "UndefinedBehaviorSanitizer: OFF")
    endif()
endif()
message(STATUS "==============================================")
