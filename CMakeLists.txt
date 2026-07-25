cmake_minimum_required(VERSION 3.24)

# Get project name from the current folder
get_filename_component(PROJECT_NAME_FROM_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} NAME)

# Replace spaces with underscores so CMake target name is valid
string(REPLACE " " "_" PROJECT_NAME_SAFE ${PROJECT_NAME_FROM_FOLDER})

project(${PROJECT_NAME_SAFE} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Download SFML 3
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)

FetchContent_MakeAvailable(SFML)

# Project folders
set(PROJECT_FOLDERS
    Include
    Source
    Assets
    Sounds
    Texts
    Data
)

# Create folders automatically
foreach(folder ${PROJECT_FOLDERS})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${folder})
endforeach()

# Create Source/main.cpp automatically if it does not exist
set(MAIN_CPP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Source/main.cpp)

if(NOT EXISTS ${MAIN_CPP_PATH})
    file(WRITE ${MAIN_CPP_PATH}
"#include <SFML/Graphics.hpp>
#include <optional>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        \"SFML 3 Setup\"
    );

    window.setFramerateLimit(60);

    sf::CircleShape circle(80.f);
    circle.setFillColor(sf::Color::Green);
    circle.setOrigin({80.f, 80.f});
    circle.setPosition({400.f, 300.f});

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(circle);
        window.display();
    }

    return 0;
}
")
endif()

# Collect source files
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
)

# Collect header files
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Include/*.hpp
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

# Include directory
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Copy asset folders next to the executable after build
foreach(folder Assets Sounds Texts Data)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${folder}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/${folder}
    )
endforeach()