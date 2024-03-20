cmake_minimum_required(VERSION 3.23)

project(rong VERSION 0.0.1 LANGUAGES CXX)

# Options.

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools)
