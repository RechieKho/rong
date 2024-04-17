cmake_minimum_required(VERSION 3.23)

project(rong VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Options.
option(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools)
