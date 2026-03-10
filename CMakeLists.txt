cmake_minimum_required(VERSION 3.20)

# NAPRAWA BŁĘDU: Dodaliśmy język C, żeby kompilator mógł przetrawić glad.c
project(NowyFolderStudioEngine VERSION 1.0 LANGUAGES C CXX)

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
# Dodajemy nasz kod C++ oraz plik glad.c z nowego folderu vendor
add_executable(${PROJECT_NAME} 
    src/main.cpp 
    thirdparty/glad/src/glad.c
)

# Mówimy kompilatorowi, gdzie leżą nagłówki GLADa
target_include_directories(${PROJECT_NAME} PRIVATE thirdparty/glad/include)

# Łączymy wszystko w całość
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    OpenGL::GL
)