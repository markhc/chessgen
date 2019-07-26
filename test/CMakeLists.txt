add_executable(unit_tests
  main.cpp
)

include(../cmake/gtest.cmake)
find_package(Threads)
target_link_libraries(unit_tests PRIVATE cppgen::cppgen ${CMAKE_THREAD_LIBS_INIT} gtest)

add_test(AllTestsInMain unit_tests)

