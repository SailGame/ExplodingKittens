aux_source_directory(. libsrc)

add_library(lib ${libsrc})
target_link_libraries(lib ${Boost_LIBRARIES} pb_lib)
target_include_directories(lib PUBLIC .)
