cmake_minimum_required(VERSION 3.16)
project(PixelPilgrimage VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------------------------------------
# Download SFML automatically
# ---------------------------------------
include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
)

FetchContent_MakeAvailable(SFML)

# ---------------------------------------

set(SOURCES
    main.cpp
    Game.cpp
    Level.cpp
    DecorationManager.cpp
    StarSpawner.cpp
)

add_executable(game ${SOURCES})

target_include_directories(game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(game PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:game>/assets
    COMMENT "Copying assets to $<TARGET_FILE_DIR:game>/assets"
)