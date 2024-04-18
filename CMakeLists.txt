cmake_minimum_required(VERSION 3.23)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)


project(rong VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Options.
option(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(FEATURE_ASSERTION OFF)

if(FEATURE_ASSERTION)
add_compile_definitions(-DFEATURE_ASSERTION)
endif(FEATURE_ASSERTION)

enable_testing()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/core)
