add_executable(attack
    level-5/src/main.cpp
)

target_include_directories(attack PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/level-5/src
)

target_link_libraries(attack PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Audio)

add_custom_command(
    TARGET attack POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/level-5/assets"
        "$<TARGET_FILE_DIR:attack>/level-5/assets"
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
