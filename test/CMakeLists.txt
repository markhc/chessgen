add_executable(unit_tests
  main.cpp
)

include(../cmake/gtest.cmake)
find_package(Threads)
target_link_libraries(unit_tests PRIVATE cppgen::cppgen gtest ${CMAKE_THREAD_LIBS_INIT})

add_test(AllTestsInMain unit_tests)

