cmake_minimum_required(VERSION 3.7)

project(chip8)

option(BUILD_TESTS "Build tests" OFF)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(
  SOURCES
  src/main.cpp
  src/chip8.cpp
  src/keyboard.cpp
  src/display.cpp
)

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})


##############
#    GTest   #
##############
if(BUILD_TESTS)
    message(STATUS "Generating tests")
    add_subdirectory(tests)
endif()