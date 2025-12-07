file(GLOB TEST_UTILS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_LIST_DIR}/utils/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/utils/*.hpp")

add_library(state-diagram-module-test STATIC ${TEST_UTILS_SOURCES})
target_link_libraries(state-diagram-module-test
    LINK_PUBLIC state-diagram-module
    LINK_PUBLIC GTest::gtest_main)

target_include_directories(state-diagram-module-test
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(state-diagram-module-test)
endif()

make_tests_from_folder(${CMAKE_CURRENT_LIST_DIR}/units
    NAME state-diagram-module-tests
    DEPENDS sc-machine::sc-builder-lib state-diagram-module-test
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(state-diagram-module-tests)
endif()
