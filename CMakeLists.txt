cmake_minimum_required(VERSION 3.20)

# NAPRAWA BŁĘDU: Dodaliśmy język C, żeby kompilator mógł przetrawić glad.c
project(PBL_ZTGK VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Znajdujemy systemową bibliotekę OpenGL
find_package(OpenGL REQUIRED)

# ==========================================
# POBIERANIE GLFW PROSTO Z GITHUBA
# ==========================================
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

# ==========================================
# NASZ SILNIK
# ==========================================

# 1. Automatyczne wyszukiwanie wszystkich plików .cpp w folderze src
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

# -- kopiowanie folderu assets do folderu z grą (tam, gdzie jest .exe)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 2. Dodajemy nasz kod oraz pliki C/CPP z thirdparty
add_executable(${PROJECT_NAME} 
    ${SRC_FILES} 
    thirdparty/glad/src/glad.c
    thirdparty/stb/stb_impl.cpp   # <-- DODANE: Ręcznie wskazujemy plik stb!
)

# 3. Mówimy kompilatorowi, gdzie leżą nagłówki (pliki .h i .hpp)
target_include_directories(${PROJECT_NAME} PRIVATE 
    thirdparty/glad/include
    thirdparty/fmod/include
    thirdparty/assimp/include
    thirdparty/glm
    thirdparty
    include
)

# 4. Łączymy wszystko w całość
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
    ${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/fmod_vc.lib
    ${CMAKE_SOURCE_DIR}/thirdparty/fmod/lib/fmodstudio_vc.lib
    ${CMAKE_SOURCE_DIR}/thirdparty/assimp/lib/assimp-vc143-mt.lib
)

# 5. AUTOMATYCZNE KOPIOWANIE PLIKÓW .dll DO FOLDERU Z GRĄ
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

