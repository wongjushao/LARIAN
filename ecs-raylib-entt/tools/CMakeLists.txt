# Test executable for World Structure
add_executable(world_test
    world_test.cpp
    ../src/world/World.cpp
)

target_include_directories(world_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../external
)

target_link_libraries(world_test
    PRIVATE
        raylib
)

if (UNIX AND NOT APPLE)
    target_link_libraries(world_test PRIVATE m pthread)
endif()
