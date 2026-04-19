file(GLOB COMMON_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")

add_library(plantuml-common ${COMMON_SOURCES})

# add_library(plantuml-common
#     DiagramBuilder.cpp
#     ParticularDiagramBuilder.cpp
#     PlantUmlPngGenerator.cpp
# )
target_link_libraries(plantuml-common
    PUBLIC
        sc-machine::sc-memory
        ps-common-lib::common-utils
)
target_include_directories(plantuml-common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..         # plantUML-lib/
        ${CMAKE_CURRENT_SOURCE_DIR}/../utils   # utils/
)
target_compile_options(plantuml-common PUBLIC -fPIC)
