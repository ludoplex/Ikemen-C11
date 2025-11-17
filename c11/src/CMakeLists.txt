# Source files for the Ikemen C11 engine
set(IKEMEN_C11_SOURCES
    engine_core.c
    main.c
)

# Create the executable
add_executable(ikemen_c11 ${IKEMEN_C11_SOURCES})

# Link against standard math library if needed
if(UNIX AND NOT APPLE)
    target_link_libraries(ikemen_c11 m)
endif()

# Set properties
set_target_properties(ikemen_c11 PROPERTIES
    OUTPUT_NAME "ikemen_c11"
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Additional compile definitions for debug builds
target_compile_definitions(ikemen_c11 PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:NDEBUG>
)
