FetchContent_MakeAvailable(googletest)

add_executable(matcher-tests MatcherTests.cc)
target_link_libraries(matcher-tests gtest gmock_main)
add_test(NAME matcher-tests COMMAND matcher-tests)

