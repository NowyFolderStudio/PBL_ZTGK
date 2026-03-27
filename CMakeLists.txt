cmake_minimum_required(VERSION 3.20)

# NAPRAWA BŁĘDU: Dodaliśmy język C, żeby kompilator mógł przetrawić glad.c
project(PBL_ZTGK VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Znajdujemy systemową bibliotekę OpenGL
find_package(OpenGL REQUIRED)

# POBIERANIE GLFW PROSTO Z GITHUBA
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

# NASZ SILNIK

# 1. Automatyczne wyszukiwanie wszystkich plików .cpp w folderze src
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

# --- NOWE: Łapiemy wszystkie pliki z folderu assets ---
file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS "assets/*")

# 2. Dodajemy nasz kod oraz pliki C/CPP z thirdparty
add_executable(${PROJECT_NAME} 
    ${SRC_FILES} 
    ${ASSET_FILES}
    thirdparty/glad/src/glad.c
    thirdparty/stb/stb_impl.cpp
    # ZMIANA: Usunąłem stąd ręczne wpisy dla Platforms. 
    # GLOB_RECURSE powyżej automatycznie znajdzie pliki w src/Platforms/!
)

# 3. Mówimy kompilatorowi, gdzie leżą nagłówki (pliki .h i .hpp)
target_include_directories(${PROJECT_NAME} PRIVATE 
    thirdparty/glad/include
    thirdparty/fmod/include
    thirdparty/assimp/include
    thirdparty/glm
    thirdparty
    include
    src    # ZMIANA: Dodałem 'src', a usunąłem 'Platforms'. Dzięki temu kompilator znajdzie i folder include, i nowe foldery w src!
)

# 4. Łączymy wszystko w całość
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
        ${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/win/fmod_vc.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/win/fmodstudio_vc.lib
        ${CMAKE_SOURCE_DIR}/thirdparty/assimp/lib/assimp-vc143-mt.lib
    )

else()
    # Ścieżki dla Linuxa
    set(FMOD_LIBS 
        "${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/linux/libfmod.so"
        "${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/linux/libfmodstudio.so"
    )
    # --- ASSIMP: FetchContent (Zamiast plików lokalnych) ---
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
        INSTALL_RPATH "${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/linux"
        BUILD_WITH_INSTALL_RPATH TRUE
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw
        OpenGL::GL
        ${FMOD_LIBS}
        assimp
        dl    
        m     
    )
endif()

# 5. AUTOMATYCZNE KOPIOWANIE PLIKÓW .dll DO FOLDERU Z GRĄ
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/thirdparty/fmod/bin/fmod.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/thirdparty/fmod/bin/fmodstudio.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/thirdparty/assimp/bin/assimp-vc143-mt.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        
    )
endif()

# 6. SYSTEM ZARZĄDZANIA ZASOBAMI (Asset Pipeline)
add_custom_target(CopyAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Kopiowanie najnowszych zasobow (Shaderow, Tekstur)..."
)

add_dependencies(${PROJECT_NAME} CopyAssets)