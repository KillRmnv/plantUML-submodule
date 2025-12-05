file(GLOB UTILS_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_library(plantuml-utils ${UTILS_SOURCES})

target_include_directories(plantuml-utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(plantuml-utils PUBLIC -fPIC)
