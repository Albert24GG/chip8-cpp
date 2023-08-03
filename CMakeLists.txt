cmake_minimum_required(VERSION 3.10)
project(chip8)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra -fstack-protector -pipe")

set(SOURCE_DIR "src")
file(GLOB SOURCE_FILES "${SOURCE_DIR}/*.cpp")
include_directories(include/)
add_executable(chip8 ${SOURCE_FILES})
target_link_libraries(chip8 ${SDL2_LIBRARIES})
