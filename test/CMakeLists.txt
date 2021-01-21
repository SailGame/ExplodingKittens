project(ExplodingKittensTest)
aux_source_directory(. DIR_SRCS)
include(GoogleTest)

add_executable(unit_test ${DIR_SRCS})

target_link_libraries(unit_test PRIVATE gtest)
target_link_libraries(unit_test PRIVATE gmock)
target_link_libraries(unit_test PRIVATE ${Boost_LIBRARIES})
target_link_libraries(unit_test PRIVATE lib)
gtest_discover_tests(unit_test PROPERTIES LABELS ExplodingKittensTest)
