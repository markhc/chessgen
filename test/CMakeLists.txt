add_executable(unit_tests
  main.cpp
)

if(ASAN)
    message(STATUS "address sanitizer enabled")

    # environment variables
    #  ASAN_OPTIONS=detect_stack_use_after_return=1
    #  ASAN_OPTIONS=check_initialization_order=1

    target_compile_options(unit_tests
      PRIVATE
        -fsanitize=address
        -g
        -fno-omit-frame-pointer)
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address)

elseif(UBSAN)
    message(STATUS "undefined behaviour sanitizer enabled")

    target_compile_options(unit_tests
    PRIVATE
        -fsanitize=undefined
        -fsanitize=integer
        -g
        -fno-omit-frame-pointer)

    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined)
endif()


include(../cmake/gtest.cmake)
find_package(Threads)
target_link_libraries(unit_tests PRIVATE cppgen::cppgen gtest ${CMAKE_THREAD_LIBS_INIT})

add_test(AllTestsInMain unit_tests)

