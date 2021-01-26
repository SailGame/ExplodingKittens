project(ExplodingKittens)
cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")

enable_testing()

add_subdirectory(externals)
add_subdirectory(lib)
add_subdirectory(test)

find_package(Boost 1.65.1 COMPONENTS program_options REQUIRED)
include_directories( ${Boost_INCLUDE_DIR})

add_executable(ExplodingKittens main.cc)

target_link_libraries(ExplodingKittens ${Boost_LIBRARIES})


include(cmake/grpc.cmake)
include(cmake/utils.cmake)
 
option(BUILD_EXPLODINGKITTENS_LIB OFF)
option(IMPORT_GRPC_BY_SUBMODULE OFF)
option(IMPORT_GRPC_BY_FETCHCONTENT OFF)

# if this file is in top directory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(BUILD_EXPLODINGKITTENS_LIB ON)
endif()

# Proto files
set(proto_dir "${CMAKE_CURRENT_SOURCE_DIR}/externals/proto")
file(GLOB proto_files ${proto_dir}/core/*.proto)
if(BUILD_EXPLODINGKITTENS_LIB)
    file(GLOB explodingkittens_proto_files ${proto_dir}/explodingkittens/*.proto)
    list(APPEND proto_files ${explodingkittens_proto_files})
endif()

# Generated pb files
set(pb_dir "${CMAKE_BINARY_DIR}/explodingkittens_pb")
file(MAKE_DIRECTORY "${pb_dir}")
add_pb_files("${proto_files}")

import_grpc()
add_custom_command(
    OUTPUT ${pb_srcs} ${pb_hdrs}
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out=generate_mock_code=true:"${pb_dir}"
    --cpp_out "${pb_dir}"
    -I "${proto_dir}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    # here should be absolute path
    ${proto_files}
    DEPENDS "${proto_files}"# "${_PROTOBUF_PROTOC}" "${_GRPC_CPP_PLUGIN_EXECUTABLE}"
)
set_source_files_properties(${pb_srcs} PROPERTIES GENERATED TRUE)

add_library(pb_lib ${pb_srcs})

target_include_directories(pb_lib PUBLIC ${CMAKE_BINARY_DIR}/explodingkittens_pb)
target_link_libraries(pb_lib PUBLIC 
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
