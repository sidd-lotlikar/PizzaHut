# Add the source files to the executable
file(GLOB_RECURSE PIZZAHUT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(PizzaHut ${PIZZAHUT_SOURCES})

target_include_directories(PizzaHut PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
