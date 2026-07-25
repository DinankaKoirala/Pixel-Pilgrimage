<<<<<<< HEAD
﻿cmake_minimum_required(VERSION 3.28)
project(PixelPilgrimage LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML 3 COMPONENTS Graphics QUIET)

if(NOT SFML_FOUND)
    message(STATUS "SFML not found locally — fetching from GitHub")
    include(FetchContent)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.1.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
    FetchContent_MakeAvailable(SFML)
else()
    message(STATUS "Using local SFML ${SFML_VERSION}")
endif()

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
=======
cmake_minimum_required(VERSION 3.10)
project(attack LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/Users/grhig/boulder_dodge/SFML-3.1.0/lib/cmake/SFML" CACHE PATH "")
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

add_executable(attack
    main.cpp
  
)

target_link_libraries(attack PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

add_custom_command(
    TARGET attack POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Data"
        "$<TARGET_FILE_DIR:attack>/Data"
)

if (WIN32)
    add_custom_command(
        TARGET attack POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:attack>
            $<TARGET_FILE_DIR:attack>
        COMMAND_EXPAND_LISTS
    )
endif()
>>>>>>> de2e7b9e3aaf4e766f5731b397724fbbce00fa9d
