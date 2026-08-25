cmake_minimum_required(VERSION 3.20)
project(hermes_package_smoke_test LANGUAGES CXX)

find_package(Hermes 1 CONFIG REQUIRED)

add_executable(hermes_package_smoke main.cpp)
target_link_libraries(hermes_package_smoke PRIVATE Hermes::hermes)
