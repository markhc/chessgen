cmake_minimum_required(VERSION 3.13)

project(chessgen 
  DESCRIPTION "Chess move generator and validator written fully in C++17"
  VERSION     1.0.0
  LANGUAGES   CXX
)

set(MAIN_TARGET "chessgen")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CHESSGEN_COMPILER_FLAGS 
    # Enable "all" warnings
    -Wall -Wextra -Weverything -Wpedantic
    # But disable some particularly annoying ones
    -Wno-covered-switch-default
    -Wno-weak-vtables
    -Wno-c++98-compat-pedantic
    -Wno-conversion
    -Wno-double-promotion
    -Wno-float-conversion
    -Wno-shorten-64-to-32
    -Wno-sign-conversion
    -Wno-padded 
    -Wno-packed
    -Wno-gcc-compat
    -Wno-comma 
    -Wno-extra-semi
  )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CHESSGEN_COMPILER_FLAGS
    # Enable "all" warnings
    -Wall -Wextra -pedantic 
    # Plus some others not included in "all"
    -Wcast-qual
    -Wconversion-null
    -Wmissing-declarations
    -Woverlength-strings
    -Wpointer-arith
    -Wunused-local-typedefs
    -Wunused-result
    -Wvarargs
    -Wvla
    -Wwrite-strings
    # except these
    -Wno-c++98-compat-pedantic 
    -Wno-weak-vtables
    -Wno-covered-switch-default
  )
else()
  # using an unsupported compiler
endif()

find_package(fmt CONFIG REQUIRED)

add_library(${MAIN_TARGET} "")
target_compile_options(${MAIN_TARGET} 
  PRIVATE 
    ${CHESSGEN_COMPILER_FLAGS}
)
target_include_directories(${MAIN_TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(${MAIN_TARGET} PUBLIC cxx_std_17)


function(target_link_libraries_system target)
  set(libs ${ARGN})
  foreach(lib ${libs})
    get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} SYSTEM PUBLIC ${lib_include_dirs})
    target_link_libraries(${target} PUBLIC ${lib})
  endforeach(lib)
endfunction(target_link_libraries_system)

target_link_libraries(${MAIN_TARGET} PUBLIC fmt::fmt fmt::fmt-header-only)
add_subdirectory(src)
add_subdirectory(test)


