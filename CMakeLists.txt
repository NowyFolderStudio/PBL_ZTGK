cmake_minimum_required(VERSION 3.20)

project(PBL_ZTGK VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/NFSEngine)

add_subdirectory(NFSEngine)
add_subdirectory(Game)