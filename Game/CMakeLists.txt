project(PBL_Game)

file(GLOB_RECURSE GAME_SRC CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "include/*.hpp"
    "include/*.h"
)

file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "assets/*")

set_source_files_properties(${ASSET_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

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

add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:RythmGame>/assets"
    COMMENT "Copying project's assets..."
)

add_dependencies(RythmGame CopyAssets)