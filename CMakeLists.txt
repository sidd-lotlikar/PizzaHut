cmake_minimum_required(VERSION 3.28)

project(PizzaHut)

# Create the executable bucket
add_executable(PizzaHut)

# Add the source files to the executable
target_sources(PizzaHut
    PRIVATE
        Main.cpp
)