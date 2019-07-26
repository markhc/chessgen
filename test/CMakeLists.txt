add_executable(unit_tests
  main.cpp
)

find_package(GTest MODULE REQUIRED)

target_link_libraries(unit_tests PRIVATE cppgen::cppgen GTest::GTest GTest::Main)

add_test(AllTestsInMain unit_tests)

