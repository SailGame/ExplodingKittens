project(ExplodingKittens)
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")

enable_testing()

add_subdirectory(externals)
add_subdirectory(lib)
add_subdirectory(test)

find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
include_directories( ${Boost_INCLUDE_DIR})

add_executable(ExplodingKittens main.cc)

target_link_libraries(ExplodingKittens ${Boost_LIBRARIES})


