cmake_minimum_required(VERSION 3.20)
project(PixelPilgrimage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use system SFML if available, otherwise download it
find_package(SFML 3.1 COMPONENTS Graphics Audio QUIET)
if(NOT SFML_FOUND)
    message(STATUS "SFML not found on system, downloading via FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
    )
    FetchContent_MakeAvailable(SFML)
else()
    message(STATUS "Using system SFML")
endif()

add_executable(PixelPilgrimage
    src/level-1/src/level1.cpp
    src/runLevel2.cpp
    src/level-1/src/audioManager.cpp
    src/level-1/src/background.cpp
    src/level-1/src/coin.cpp
    src/level-1/src/enemy.cpp
    src/level-1/src/player.cpp
    src/level-1/src/tilemap.cpp
)
target_include_directories(PixelPilgrimage PRIVATE src/level-1/src/Header src)
target_link_libraries(PixelPilgrimage PRIVATE SFML::Graphics SFML::Audio)

# Load assets directly from source, no copying needed
set_target_properties(PixelPilgrimage PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/level-1"
)

add_executable(PixelPilgrimage_level2
    src/level-2/src/level2.cpp
    src/level-2/src/Game.cpp
    src/level-2/src/Decorationmanager.cpp
    src/level-2/src/Starspawner.cpp
    src/level-2/src/Level.cpp
)
target_include_directories(PixelPilgrimage_level2 PRIVATE src/level-2/src)
target_link_libraries(PixelPilgrimage_level2 PRIVATE SFML::Graphics SFML::Audio)

# Copy Level 2 assets next to its exe
add_custom_command(TARGET PixelPilgrimage_level2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/src/level-2/assets"
    "$<TARGET_FILE_DIR:PixelPilgrimage_level2>/assets"
)

# Copy Level 2 exe next to Level 1 exe so runLevel2() can find it
add_custom_command(TARGET PixelPilgrimage POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:PixelPilgrimage_level2>"
    "$<TARGET_FILE_DIR:PixelPilgrimage>/PixelPilgrimage_level2.exe"
)