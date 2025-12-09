file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "utils/*.cpp" "utils/*.hpp"
    "builder/*.cpp" "builder/*.hpp"

)
find_program(JAVA_EXECUTABLE java)

if(NOT JAVA_EXECUTABLE)
    message(FATAL_ERROR "Java runtime (java) not found. Install Java (JRE/JDK).")
endif()

message(STATUS "Found Java: ${JAVA_EXECUTABLE}")
add_library(er-diagram-module SHARED ${SOURCES})
target_link_libraries(er-diagram-module
LINK_PUBLIC sc-machine::sc-core
LINK_PUBLIC sc-machine::sc-memory
LINK_PUBLIC sc-machine::sc-agents-common
LINK_PUBLIC scl-machine::inference
LINK_PUBLIC ps-common-lib::common-utils
LINK_PUBLIC sc-builder-local
    LINK_PUBLIC plantuml-common
    LINK_PUBLIC plantuml-utils
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
