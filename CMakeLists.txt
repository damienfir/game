cmake_minimum_required(VERSION 3.20)
project(game)

set(CMAKE_CXX_STANDARD 20)

find_package(FreeGLUT REQUIRED)

add_executable(game main.cpp)
target_link_libraries(game FreeGLUT::freeglut GLEW)