FetchContent_MakeAvailable(googletest)

add_executable(matcher-tests MatcherTests.cc)
target_link_libraries(matcher-tests gtest gmock_main eds)

add_test(NAME matcher-tests COMMAND matcher-tests)

