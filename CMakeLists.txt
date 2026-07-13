cmake_minimum_required(VERSION 3.16)
project(NightWinterScene VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

add_executable(game main.cpp src/level4.cpp)

target_link_libraries(game PRIVATE SFML::Graphics SFML::Window SFML::System)

# Copy assets to build directory
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:game>/assets"
    COMMENT "Copying assets to build directory")

# Copy SFML DLLs to build directory (generator expressions for debug/release)
add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SFML::System>"
        "$<TARGET_FILE_DIR:game>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SFML::Window>"
        "$<TARGET_FILE_DIR:game>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SFML::Graphics>"
        "$<TARGET_FILE_DIR:game>"
    COMMENT "Copying SFML DLLs to build directory")


