# Source files for the engine library
set(ENGINE_SOURCES
    engine.c
)

# Create the engine static library
add_library(ikemen_engine STATIC ${ENGINE_SOURCES})

target_include_directories(ikemen_engine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Main executable
add_executable(ikemen_c11 main.c)

target_link_libraries(ikemen_c11
    PRIVATE ikemen_engine
)

# Installation rules
install(TARGETS ikemen_c11 ikemen_engine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
