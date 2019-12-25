add_executable(unit_tests
  test_bitboard.cpp
  test_full_games.cpp
  main.cpp
)

if(CHESSGEN_ASAN)
    message(STATUS "ASAN enabled")

    target_compile_options(unit_tests
      PRIVATE
        -fsanitize=address
        -g
        -fno-omit-frame-pointer)
    
    set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address)
elseif(CHESSGEN_UBSAN)
    message(STATUS "UBSAN enabled")

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
target_link_libraries(unit_tests PRIVATE chessgen::chessgen gtest ${CMAKE_THREAD_LIBS_INIT})

add_test(AllTestsInMain unit_tests)

