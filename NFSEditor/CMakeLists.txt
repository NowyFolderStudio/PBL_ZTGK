project(NFSEditor)

file(GLOB_RECURSE EDITOR_SRC CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "include/*.hpp"
    "include/*.h"
)

add_executable(NFSEditor ${EDITOR_SRC})

set_target_properties(NFSEditor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(NFSEditor PRIVATE NFSEngine)

target_include_directories(NFSEditor PRIVATE
    src
    include
)