file(GLOB TEST_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/utils/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/utils/*.hpp")

add_library(use-case-diagram-module-test STATIC ${TEST_UTILS_SOURCES})
target_link_libraries(use-case-diagram-module-test
    LINK_PUBLIC use-case-diagram-module
    LINK_PUBLIC GTest::gtest_main)

target_include_directories(use-case-diagram-module-test
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(use-case-diagram-module-test)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME use-case-diagram-module-tests
    DEPENDS sc-machine::sc-builder-lib use-case-diagram-module-test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(use-case-diagram-module-tests)
endif()
