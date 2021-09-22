cmake_minimum_required(VERSION 3.20)
project(game)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(game main.cpp shader.cpp buffer.cpp logging.cpp maths.cpp camera.cpp mesh.cpp mesh.h axes.cpp axes.h)
target_link_libraries(game glfw GLEW OpenGL::GL)