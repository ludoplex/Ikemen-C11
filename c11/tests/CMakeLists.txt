# Test executable
add_executable(test_engine test_engine.c)

target_link_libraries(test_engine
    PRIVATE ikemen_engine
)

target_include_directories(test_engine
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Add test to CTest
add_test(
    NAME EngineTests
    COMMAND test_engine
)

# Set test properties
set_tests_properties(EngineTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)
