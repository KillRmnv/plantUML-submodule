# Файл: plantUML-submodule/utils/sc-builder-local/CMakeLists.txt

set(SC_BUILDER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB SOURCES CONFIGURE_DEPENDS
    "src/*.cpp" "src/*.hpp"
    "include/sc-builder-local/*.hpp"
)
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

find_package(LibXml2 REQUIRED)

# *** ИЗМЕНЕНИЕ: sc-builder-lib -> sc-builder-local ***
add_library(sc-builder-local STATIC ${SOURCES}) # Используем STATIC, чтобы избежать проблем с экспортом символов
target_compile_options(sc-builder-local PUBLIC -fPIC)
target_link_libraries(sc-builder-local
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config
    LINK_PRIVATE LibXml2::LibXml2
)
target_include_directories(sc-builder-local
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PUBLIC $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Оставьте sc-builder (исполняемый файл), он вам не нужен, но его нужно скомпилировать, если вы не удалили main.cpp
add_executable(sc-builder "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")

# *** ИЗМЕНЕНИЕ: sc-builder-lib -> sc-builder-local ***
target_link_libraries(sc-builder LINK_PRIVATE sc-builder-local
LINK_PUBLIC sc-machine::sc-core
LINK_PUBLIC sc-machine::sc-memory) 

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(sc-builder)
    target_clangformat_setup(sc-builder-local)
endif()

