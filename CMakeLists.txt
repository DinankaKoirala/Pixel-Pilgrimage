cmake_minimum_required(VERSION 3.28)

project(PixelPilgrimage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/Users/Desktop-LOQ/Documents/libaries/SFML-3.1.0/lib/cmake/SFML")

find_package(SFML REQUIRED COMPONENTS Graphics)

add_executable(main
    src/main.cpp
    src/player.cpp
    src/tilemap.cpp
    src/enemy.cpp
    src/background.cpp
)

target_include_directories(main PRIVATE src/Header)

target_link_libraries(main PRIVATE SFML::Graphics)

# Automatically copy all SFML DLLs after building
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "C:/Users/Desktop-LOQ/Documents/libaries/SFML-3.1.0/bin"
    "$<TARGET_FILE_DIR:main>"
)