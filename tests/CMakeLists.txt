add_executable(PizzaHutTests ResultTests.cpp)

target_include_directories(PizzaHutTests PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(PizzaHutTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(PizzaHutTests)
