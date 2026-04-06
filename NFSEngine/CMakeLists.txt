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

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

add_library(${PROJECT_NAME} STATIC
    ${SRC_FILES} 
    thirdparty/glad/src/glad.c
    thirdparty/stb/stb_impl.cpp
        include/Core/KeyCodes.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    thirdparty/glad/include
    thirdparty/fmod/include
    thirdparty/assimp/include
    thirdparty/glm
    thirdparty
    include
    src
)

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