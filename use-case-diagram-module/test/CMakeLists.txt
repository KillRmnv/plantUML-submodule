# Файл: problem-solver/cxx/plantUML-submodule/use-case-diagram-module/test/CMakeLists.txt

# Включаем определение пользовательских команд тестирования, таких как make_tests_from_folder
# Предполагается, что эта строка находится в начале, как мы обсуждали ранее:
include(${PROJECT_SOURCE_DIR}/cmake/tests.cmake) 
find_package(LibXml2 REQUIRED)
# 1. ИСПРАВЛЕНИЕ: Используем существующий каталог units для сбора исходников
file(GLOB TEST_UNITS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/units/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/units/*.hpp")
    
include(CTest)
include(GoogleTest)
find_package(GTest REQUIRED)
# include(${CMAKE_MODULE_PATH}/tests.cmake) <-- УДАЛЕНО: Эта строка тут не нужна

# 2. ВОССТАНОВЛЕНИЕ: Определяем тестовую библиотеку
add_library(use-case-diagram-module-test STATIC ${TEST_UNITS_SOURCES})

# Связывание тестовой библиотеки со всеми необходимыми зависимостями
target_link_libraries(use-case-diagram-module-test
    LINK_PUBLIC use-case-diagram-module
    LINK_PUBLIC sc-builder-local
    LINK_PUBLIC sc-memory
    LINK_PUBLIC sc-config
    LINK_PUBLIC sc-core
    LINK_PUBLIC LibXml2::LibXml2
    LINK_PUBLIC GTest::gtest_main
)

target_include_directories(use-case-diagram-module-test
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(use-case-diagram-module-test)
endif()

# 3. Создание тестов с использованием пользовательской функции
make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME use-case-diagram-module-tests
    DEPENDS
        use-case-diagram-module-test
        sc-builder-local
)


if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(use-case-diagram-module-tests)
endif()