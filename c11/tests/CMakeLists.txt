# Tests for the Ikemen C11 engine

# Test: Engine Core
add_executable(test_engine_core
    test_engine_core.c
    ../src/engine_core.c
)

target_include_directories(test_engine_core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Link against math library if needed
if(UNIX AND NOT APPLE)
    target_link_libraries(test_engine_core m)
endif()

set_target_properties(test_engine_core PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
)

# Register test with CTest
add_test(NAME EngineCore COMMAND test_engine_core)

# Set test properties
set_tests_properties(EngineCore PROPERTIES
    LABELS "unit"
    TIMEOUT 10
)
