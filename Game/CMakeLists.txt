project(PBL_Game)

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "assets/*")

add_executable(RythmGame ${GAME_SRC} ${ASSET_FILES})

target_link_libraries(RythmGame PRIVATE NFSEngine)

target_include_directories(RythmGame PRIVATE
    src
    include
)

if(WIN32)
    add_custom_command(TARGET RythmGame POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ENGINE_DIR}/thirdparty/fmod/bin/fmod.dll"
        "$<TARGET_FILE_DIR:RythmGame>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ENGINE_DIR}/thirdparty/fmod/bin/fmodstudio.dll"
        "$<TARGET_FILE_DIR:RythmGame>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ENGINE_DIR}/thirdparty/assimp/bin/assimp-vc143-mt.dll"
        "$<TARGET_FILE_DIR:RythmGame>"
    )
endif()

# Kopiowanie Assetów
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:RythmGame>/assets"
    COMMENT "Kopiowanie najnowszych zasobow (Shaderow, Tekstur)..."
)

add_dependencies(RythmGame CopyAssets)