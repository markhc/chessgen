include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
  test_bitboard.cpp
  test_full_games.cpp
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

find_package(Threads)
target_link_libraries(unit_tests PRIVATE chessgen::chessgen GTest::gtest_main ${CMAKE_THREAD_LIBS_INIT})

include(GoogleTest)
gtest_discover_tests(unit_tests)

# add_test(AllTestsInMain unit_tests)
