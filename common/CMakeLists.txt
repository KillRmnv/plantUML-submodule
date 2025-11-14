file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_library(plantuml-common ${COMMON_SOURCES})

target_include_directories(plantuml-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
