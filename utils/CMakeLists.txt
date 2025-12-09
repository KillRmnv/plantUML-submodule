# Файл: plantUML-submodule/utils/CMakeLists.txt

# 1. Собираем только base64 и другие общие утилиты, не относящиеся к sc-builder-local
file(GLOB UTILS_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
# Исключаем подкаталог sc-builder-local из общего GLOB
list(REMOVE_ITEM UTILS_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/sc-builder-local"
    "${CMAKE_CURRENT_SOURCE_DIR}/sc-builder-local/CMakeLists.txt" # Удаляем, чтобы не мешало
)
# Удалите jar-файл из списка исходников, если он туда попал
list(REMOVE_ITEM UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/plantuml.1.2023.7.jar")


add_library(plantuml-utils STATIC ${UTILS_SOURCES})

target_include_directories(plantuml-utils
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_options(plantuml-utils PUBLIC -fPIC)

# Не добавляйте здесь add_subdirectory(sc-builder-local), это уже сделано в корневом CMakeLists.txt