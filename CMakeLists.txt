cmake_minimum_required(VERSION 3.10)
project(Ikemen_C11 C)

# Set C11 standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable all warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Source files
set(SOURCES
    src/c11/main.c
    src/c11/config.c
    src/c11/system.c
    src/c11/lua_interface.c
)

# Create executable
add_executable(Ikemen_C11 ${SOURCES})

# Platform-specific settings
if(WIN32)
    # Windows-specific libraries
    target_link_libraries(Ikemen_C11)
elseif(APPLE)
    # macOS-specific libraries
    target_link_libraries(Ikemen_C11)
else()
    # Linux-specific libraries
    target_link_libraries(Ikemen_C11 m)
endif()

# TODO: Add dependencies when implementing full features
# Find and link Lua
# find_package(Lua REQUIRED)
# target_include_directories(Ikemen_C11 PRIVATE ${LUA_INCLUDE_DIR})
# target_link_libraries(Ikemen_C11 ${LUA_LIBRARIES})

# Find and link OpenGL/GLFW
# find_package(OpenGL REQUIRED)
# find_package(glfw3 REQUIRED)
# target_link_libraries(Ikemen_C11 OpenGL::GL glfw)

# Find and link FFmpeg
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswscale)
# target_include_directories(Ikemen_C11 PRIVATE ${FFMPEG_INCLUDE_DIRS})
# target_link_directories(Ikemen_C11 PRIVATE ${FFMPEG_LIBRARY_DIRS})
# target_link_libraries(Ikemen_C11 ${FFMPEG_LIBRARIES})

# Install target
install(TARGETS Ikemen_C11 DESTINATION bin)
