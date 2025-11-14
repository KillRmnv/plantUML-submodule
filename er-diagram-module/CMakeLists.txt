file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "structures/*.cpp" "structures/*.hpp"
    "searcher/*.cpp" "searcher/*.hpp"
    "utils/*.cpp" "utils/*.hpp"
)

add_library(er-diagram-module SHARED ${SOURCES})
target_link_libraries(er-diagram-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
    LINK_PUBLIC scl-machine::inference
    LINK_PUBLIC ps-common-lib::common-utils
)
target_include_directories(er-diagram-module
    PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
set_target_properties(er-diagram-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(er-diagram-module)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(test)
endif()
