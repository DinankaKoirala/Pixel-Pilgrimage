cmake_minimum_required(VERSION 3.20)
project(PixelPilgrimage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML REQUIRED COMPONENTS Graphics Audio)

add_executable(PixelPilgrimage
    main.cpp
    runLevel2.cpp
    level-1/src/audioManager.cpp
    level-1/src/background.cpp
    level-1/src/coin.cpp
    level-1/src/enemy.cpp
    level-1/src/player.cpp
    level-1/src/tilemap.cpp
)
target_include_directories(PixelPilgrimage PRIVATE level-1/src/Header)
target_link_libraries(PixelPilgrimage PRIVATE SFML::Graphics SFML::Audio)

add_executable(PixelPilgrimage_level2
    level-2/src/level2.cpp
    level-2/src/Game.cpp
    level-2/src/Decorationmanager.cpp
    level-2/src/Starspawner.cpp
    level-2/src/Level.cpp
)
target_include_directories(PixelPilgrimage_level2 PRIVATE level-2/src)
target_link_libraries(PixelPilgrimage_level2 PRIVATE SFML::Graphics SFML::Audio)

add_custom_command(TARGET PixelPilgrimage_level2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/level-2/assets"
    "$<TARGET_FILE_DIR:PixelPilgrimage_level2>/assets"
)

if(WIN32)
    get_filename_component(SFML_ROOT "${SFML_DIR}" DIRECTORY)
    get_filename_component(SFML_ROOT "${SFML_ROOT}" DIRECTORY)
    get_filename_component(SFML_ROOT "${SFML_ROOT}" DIRECTORY)
    add_custom_command(TARGET PixelPilgrimage POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_ROOT}/bin"
        "$<TARGET_FILE_DIR:PixelPilgrimage>"
    )
    add_custom_command(TARGET PixelPilgrimage_level2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SFML_ROOT}/bin"
        "$<TARGET_FILE_DIR:PixelPilgrimage_level2>"
    )
endif()
