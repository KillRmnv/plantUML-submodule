file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "common/*.cpp" "common/*.hpp"
    "utils/*.cpp" "utils/*.h"

)
find_program(JAVA_EXECUTABLE java)

if(NOT JAVA_EXECUTABLE)
    message(FATAL_ERROR "Java runtime (java) not found. Install Java (JRE/JDK).")
endif()

message(STATUS "Found Java: ${JAVA_EXECUTABLE}")

add_library(plantUML-submodule SHARED ${SOURCES})

target_link_libraries(plantUML-submodule
    LINK_PUBLIC sc-machine::sc-core
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
    LINK_PUBLIC scl-machine::inference
    LINK_PUBLIC ps-common-lib::common-utils
)
target_include_directories(plantUML-submodule
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(plantUML-submodule PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(plantUML-submodule)
endif()

if(${SC_BUILD_TESTS})
    add_subdirectory(test)
endif()
