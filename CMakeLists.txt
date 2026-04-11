cmake_minimum_required(VERSION 3.20)

project(PBL_ZTGK VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/NFSEngine)
# set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)

# # --- CLANG-FORMAT CONFIG ---

# find_program(CLANG_FORMAT_EXE NAMES "clang-format")

# if(CLANG_FORMAT_EXE)
#     message(STATUS "Clang-format found: ${CLANG_FORMAT_EXE}")

#     file(GLOB_RECURSE ALL_SOURCE_FILES 
#         "${ENGINE_DIR}/src/*.cpp" "${ENGINE_DIR}/include/*.h" "${ENGINE_DIR}/include/*.hpp"
#         "${GAME_DIR}/*.cpp" "${GAME_DIR}/*.h" "${GAME_DIR}/*.hpp"
#     )

#     add_custom_target(format
#         COMMAND ${CLANG_FORMAT_EXE} -style=file -i ${ALL_SOURCE_FILES}
#         COMMENT "Fixing code format for NFSEngine and Game subprojects..."
#     )

# else()
#     message(WARNING "Clang-format not found! Target 'format' is not available.")
# endif()

# find_program(CLANG_TIDY_EXE NAMES "clang-tidy")

# if(CLANG_TIDY_EXE)
#     message(STATUS "Clang-Tidy found: ${CLANG_TIDY_EXE}")
# else()
#     message(WARNING "Clang not found. Code will not be analized.")
# endif()

add_subdirectory(NFSEngine)
add_subdirectory(Game)