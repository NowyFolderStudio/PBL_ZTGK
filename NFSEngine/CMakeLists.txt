project(NFSEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenGL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8 
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Turning off tests and examples in spdlog project
set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/spdlog)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "include/*.hpp"
    "include/*.h"
)

# ImGui docking
set(IMGUI_DIR thirdparty/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set_source_files_properties(
    thirdparty/glad/src/glad.c
    thirdparty/stb/stb_impl.cpp
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON
    )

add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES}
    ${IMGUI_SOURCES}
    thirdparty/glad/src/glad.c
    thirdparty/stb/stb_impl.cpp
)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NFS_DEBUG)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC NFS_RELEASE)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    thirdparty/glad/include
    thirdparty/fmod/include
    thirdparty/assimp/include
    thirdparty/glm
    thirdparty
    include
    src
)

# Linking spdlog to the engine (spdlog v1.x)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        glfw
        OpenGL::GL
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmod/lib/win/fmod_vc.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmod/lib/win/fmodstudio_vc.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/assimp/lib/assimp-vc143-mt.lib
    )

else()
    set(FMOD_LIBS 
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmod/lib/linux/libfmod.so"
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmod/lib/linux/libfmodstudio.so"
    )
    FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v5.3.1
    )
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE) 

    FetchContent_MakeAvailable(assimp)
    
    set_target_properties(${PROJECT_NAME} PROPERTIES 
        INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/fmod/lib/linux"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    target_link_libraries(${PROJECT_NAME} PUBLIC
        glfw
        OpenGL::GL
        ${FMOD_LIBS}
        assimp
        dl    
        m     
    )
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE include/nfspch.h)