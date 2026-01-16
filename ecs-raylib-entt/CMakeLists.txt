cmake_minimum_required(VERSION 3.16)
project(ecs_raylib_entt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)

# Add executable (source files live in src/)
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
	src/*.cpp
)

add_executable(ecs_raylib_entt
	${SRC_FILES}
)

# Include paths for our own headers and external libraries (like EnTT)
target_include_directories(ecs_raylib_entt
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CURRENT_SOURCE_DIR}/external
		${CMAKE_CURRENT_SOURCE_DIR}/external/entt
)

# --- raylib local build (no sudo, no system dependency) ---

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Don't build raylib games" FORCE)

FetchContent_Declare(
	raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG        5.0
)

FetchContent_MakeAvailable(raylib)

target_link_libraries(ecs_raylib_entt
	PRIVATE
		raylib
)

if (BUILD_WARNINGS_AS_ERRORS)
	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
		target_compile_options(ecs_raylib_entt PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endif()

if (UNIX AND NOT APPLE)
	target_link_libraries(ecs_raylib_entt PRIVATE m pthread)
endif()

# Add tools subdirectory
add_subdirectory(tools)
