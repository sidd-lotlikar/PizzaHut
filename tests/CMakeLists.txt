# Automatically include tests placed in this directory or a subdirectory.
file(GLOB_RECURSE PIZZAHUT_TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_executable(PizzaHutTests
    ${PIZZAHUT_TEST_SOURCES}
    ${PROJECT_SOURCE_DIR}/src/FileHandle.cpp
)

target_include_directories(PizzaHutTests PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(PizzaHutTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(PizzaHutTests)
