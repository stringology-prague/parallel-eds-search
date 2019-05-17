add_library(eds ElasticDegenerateString.cc)

add_executable(eds-cli eds-cli.cc)
target_link_libraries(eds-cli eds)

target_link_libraries(eds-cli ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SERIALIZATION_LIBRARY})
