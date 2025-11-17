# Ikemen C11 Engine - CMake Build Configuration
#
# This build system enforces C11 standard compliance and builds:
# - A library containing the core engine functionality
# - An executable for the main engine bootstrap
# - Test executables integrated with CTest
#
# Out-of-tree builds are recommended:
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
#   ctest

cmake_minimum_required(VERSION 3.10)

project(IkemenC11
    VERSION 0.1.0
    DESCRIPTION "Ikemen C11 Engine - MUGEN-compatible fighting game engine"
    LANGUAGES C
)

# Enforce C11 standard with no compiler extensions
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Enable strict warnings on supported compilers
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    # Additional helpful warnings
    add_compile_options(-Wshadow -Wpointer-arith -Wcast-align)
    add_compile_options(-Wstrict-prototypes -Wmissing-prototypes)
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Core engine library
add_library(ikemen_engine STATIC
    src/engine.c
)

target_include_directories(ikemen_engine PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Main executable
add_executable(ikemen_c11
    src/main.c
)

target_link_libraries(ikemen_c11 PRIVATE ikemen_engine)

# Enable testing
enable_testing()

# Test: Version functions
add_executable(test_version
    tests/test_version.c
)
target_link_libraries(test_version PRIVATE ikemen_engine)
add_test(NAME VersionFunctions COMMAND test_version)

# Test: Asset compatibility
add_executable(test_asset_compat
    tests/test_asset_compat.c
)
target_link_libraries(test_asset_compat PRIVATE ikemen_engine)
add_test(NAME AssetCompatibility COMMAND test_asset_compat)

# Installation rules (optional, for packaging)
install(TARGETS ikemen_c11 ikemen_engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES include/ikemen_engine.h
    DESTINATION include
)

# Print configuration summary
message(STATUS "")
message(STATUS "Ikemen C11 Engine Configuration:")
message(STATUS "  Version:        ${PROJECT_VERSION}")
message(STATUS "  Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler:     ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  C Standard:     C${CMAKE_C_STANDARD}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
