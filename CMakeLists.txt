cmake_minimum_required(VERSION 3.13)

option(CPPGEN_INSTALL "Generate the install target." ON)
option(CPPGEN_TEST "Generate the test target." ON)

project(cppgen CXX)

set(CPPGEN_VERSION 1.0.0)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

message(STATUS "Version: ${CPPGEN_VERSION}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CPPGEN_COMPILER_FLAGS 
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
    -Wno-undefined-func-template
  )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CPPGEN_COMPILER_FLAGS
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

function(add_headers VAR)
  set(headers ${${VAR}})
  foreach (header ${ARGN})
    set(headers ${headers} include/cppgen/${header})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

function(add_sources VAR)
  set(sources ${${VAR}})
  foreach (source ${ARGN})
    set(sources ${sources} src/${source})
  endforeach()
  set(${VAR} ${headers} PARENT_SCOPE)
endfunction()

add_headers(CPPGEN_HEADERS 
            attacks.hpp bitboard.hpp board.hpp 
            config.hpp helpers.hpp move.hpp 
            movegen.hpp platform.hpp types.hpp)

#add_sources(CPPGEN_SOURCES 
#            attacks.cpp bitboard.cpp board.cpp 
#            move.cpp movegen.cpp)

add_library(cppgen src/attacks.cpp src/bitboard.cpp src/board.cpp src/move.cpp src/movegen.cpp)
add_library(cppgen::cppgen ALIAS cppgen)

target_compile_features(cppgen PUBLIC cxx_std_17)
target_compile_options(cppgen 
  PRIVATE 
    ${CPPGEN_COMPILER_FLAGS}
)
target_include_directories(cppgen PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
set_target_properties(cppgen PROPERTIES
  VERSION ${CPPGEN_VERSION} 
  DEBUG_POSTFIX d)
  
# Install targets.
if(CPPGEN_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  set(CPPGEN_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/cppgen CACHE STRING
      "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")
  set(version_config ${PROJECT_BINARY_DIR}/cppgen-config-version.cmake)
  set(project_config ${PROJECT_BINARY_DIR}/cppgen-config.cmake)
  set(pkgconfig ${PROJECT_BINARY_DIR}/cppgen.pc)
  set(targets_export_name cppgen-targets)

  set (INSTALL_TARGETS cppgen)

  set(CPPGEN_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING
      "Installation directory for libraries, relative to ${CMAKE_INSTALL_PREFIX}.")

  set(CPPGEN_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR}/cppgen CACHE STRING
      "Installation directory for include files, relative to ${CMAKE_INSTALL_PREFIX}.")

  set(CPPGEN_PKGCONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE PATH
      "Installation directory for pkgconfig (.pc) files, relative to ${CMAKE_INSTALL_PREFIX}.")

  write_basic_package_version_file(
    ${version_config}
    VERSION ${CPPGEN_VERSION}
    COMPATIBILITY AnyNewerVersion)
    
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cppgen.pc.in"
    "${pkgconfig}"
    @ONLY)

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/cppgen-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${CPPGEN_CMAKE_DIR})

  # Use a namespace because CMake provides better diagnostics for namespaced
  # imported targets.
  export(TARGETS ${INSTALL_TARGETS}
         FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)

  # Install version, config and target files.
  install(FILES ${project_config} ${version_config}
          DESTINATION ${CPPGEN_CMAKE_DIR})

  install(EXPORT ${targets_export_name} 
          DESTINATION ${CPPGEN_CMAKE_DIR}
          NAMESPACE cppgen::)

  # Install the library and headers.
  install(TARGETS ${INSTALL_TARGETS} 
          EXPORT ${targets_export_name}
          DESTINATION ${CPPGEN_LIB_DIR})

  install(FILES ${CPPGEN_HEADERS} DESTINATION ${CPPGEN_INC_DIR})
  install(FILES "${pkgconfig}" DESTINATION "${CPPGEN_PKGCONFIG_DIR}")
endif ()

if(CPPGEN_TEST)
  enable_testing()
  add_subdirectory(test)
endif ()


