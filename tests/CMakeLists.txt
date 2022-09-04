cmake_minimum_required(VERSION 3.7)

project(chip8)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})


set(
  TEST_SOURCES
  ./chip8.test.cpp
  ../src/chip8.cpp
)

add_executable(${PROJECT_NAME}_test ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}_test ${GTEST_LIBRARIES})