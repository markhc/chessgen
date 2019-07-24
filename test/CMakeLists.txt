add_executable(unit_tests
  main.cpp
)

find_package(GTest MODULE REQUIRED)
target_link_libraries(unit_tests PRIVATE ${MAIN_TARGET} GTest::GTest GTest::Main)
#enable_testing()
#add_test(AllTestsInMain unit_tests)

