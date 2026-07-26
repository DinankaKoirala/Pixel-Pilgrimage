cmake_minimum_required(VERSION 3.20)
project(PixelPilgrimage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# ── helper macro to copy SFML DLLs ───────────────────────────────────────────
macro(copy_sfml_dlls TARGET_NAME)
    if(WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SFML::Graphics>
                $<TARGET_FILE:SFML::Audio>
                $<TARGET_FILE:SFML::Window>
                $<TARGET_FILE:SFML::System>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
    endif()
endmacro()

# ── Main integrated executable ───────────────────────────────────────────────
add_executable(PixelPilgrimage
    # Level 1
    src/level-1/src/level1.cpp
    src/level-1/src/audioManager.cpp
    src/level-1/src/background.cpp
    src/level-1/src/coin.cpp
    src/level-1/src/enemy.cpp
    src/level-1/src/player.cpp
    src/level-1/src/tilemap.cpp
    # Level 2
    src/level-2/src/Game.cpp
    src/level-2/src/Decorationmanager.cpp
    src/level-2/src/Starspawner.cpp
    src/level-2/src/Level.cpp
    # Level 3
    src/level-3/Game.cpp
    src/level-3/Knight.cpp
    src/level-3/Villain.cpp
    src/level-3/Princess.cpp
    # Level 4
    src/level-4/src/level4.cpp
    src/level-4/src/Game.cpp
    # Runners
    src/runLevel2.cpp
    src/runLevel3.cpp
    src/runLevel4.cpp
    # Shared
    src/shared/death-screen/DeathScreen.cpp
    src/shared/start-screen/StartScreen.cpp
    src/shared/map/MapScreen.cpp
)
target_include_directories(PixelPilgrimage PRIVATE
    src
    src/shared
    src/level-1/src/Header
    src/level-2/src
    src/level-3
    src/level-4/src
    src/level-4/src/Header
)
target_link_libraries(PixelPilgrimage PRIVATE SFML::Graphics SFML::Audio)
set_target_properties(PixelPilgrimage PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/level-1"
)
copy_sfml_dlls(PixelPilgrimage)
