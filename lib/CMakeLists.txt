aux_source_directory(. lib)

add_library(lib ${lib})
target_link_libraries(lib ${Boost_LIBRARIES})