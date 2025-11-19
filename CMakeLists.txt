file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "common/*.cpp" "common/*.hpp"

)

add_library(plantUML-lib SHARED ${SOURCES})
find_package(CURL REQUIRED)

target_link_libraries(plantUML-lib
    LINK_PUBLIC sc-machine::sc-core
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
    LINK_PUBLIC scl-machine::inference
    LINK_PUBLIC ps-common-lib::common-utils
    LINK_PUBLIC CURL::libcurl
)
target_include_directories(plantUML-lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(plantUML-lib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(plantUML-lib)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(test)
endif()
