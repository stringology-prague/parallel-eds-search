add_executable(eds-cli eds-cli.cc)

target_link_libraries(eds-cli ${Boost_PROGRAM_OPTIONS_LIBRARY})
