file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "structures/*.cpp" "structures/*.hpp"
    "searcher/*.cpp" "searcher/*.hpp"
    "utils/*.cpp" "utils/*.hpp"
    "builder/*.cpp" "builder/*.hpp"
)

add_library(state-diagram-module SHARED ${SOURCES})
target_link_libraries(state-diagram-module
    LINK_PUBLIC sc-machine::sc-core
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
    LINK_PUBLIC scl-machine::inference
    LINK_PUBLIC ps-common-lib::common-utils
    LINK_PUBLIC sc-builder-local
    LINK_PUBLIC plantuml-common
    LINK_PUBLIC plantuml-utils
)
target_include_directories(state-diagram-module
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        # ${PROJECT_SOURCE_DIR}/utils/sc-builder/include
        # ${PROJECT_SOURCE_DIR}/utils/sc-builder/src
)
set_target_properties(state-diagram-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(state-diagram-module)
endif()
add_subdirectory(test)
if(${SC_BUILD_TESTS})
add_subdirectory(test)
endif()
