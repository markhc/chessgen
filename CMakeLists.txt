cmake_minimum_required(VERSION 3.13)

project(chessgen CXX)

set(CHESSGEN_VERSION 1.0.0)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Determine if chessgen is built as a subproject (using add_subdirectory)
# or if it is the master project.
set(MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

option(CHESSGEN_INSTALL "Generate the install target." ${MASTER_PROJECT})
option(CHESSGEN_TESTS   "Generate the test target." OFF)
option(CHESSGEN_ASAN    "Enable address sanitizer" OFF)
option(CHESSGEN_UBSAN   "Enable undefined behaviour sanitizer" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
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
    -Wno-undefined-func-template
  )
  
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
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
elseif (MSVC)
  set(CHESSGEN_COMPILER_FLAGS /W3)
endif ()

add_library(chessgen 
            src/attacks.cpp 
            src/bitboard.cpp 
            src/board.cpp
            src/board_state.cpp
            src/movegen.cpp
            src/san.cpp)

add_library(chessgen::chessgen ALIAS chessgen)

target_compile_features(chessgen PUBLIC cxx_std_17)
target_compile_options(chessgen 
  PRIVATE 
    ${CHESSGEN_COMPILER_FLAGS}
)
target_include_directories(chessgen PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
set_target_properties(chessgen PROPERTIES
  VERSION ${CHESSGEN_VERSION} 
  DEBUG_POSTFIX d)
  
# Install targets.
if(CHESSGEN_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)
  set(CHESSGEN_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/chessgen CACHE STRING
      "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")
  set(version_config ${PROJECT_BINARY_DIR}/chessgen-config-version.cmake)
  set(project_config ${PROJECT_BINARY_DIR}/chessgen-config.cmake)
  set(pkgconfig ${PROJECT_BINARY_DIR}/chessgen.pc)
  set(targets_export_name chessgen-targets)

  set (INSTALL_TARGETS chessgen)

  set(CHESSGEN_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE STRING
      "Installation directory for libraries, relative to ${CMAKE_INSTALL_PREFIX}.")

  set(CHESSGEN_INC_DIR ${CMAKE_INSTALL_INCLUDEDIR}/chessgen CACHE STRING
      "Installation directory for include files, relative to ${CMAKE_INSTALL_PREFIX}.")

  set(CHESSGEN_PKGCONFIG_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE PATH
      "Installation directory for pkgconfig (.pc) files, relative to ${CMAKE_INSTALL_PREFIX}.")

  write_basic_package_version_file(
    ${version_config}
    VERSION ${CHESSGEN_VERSION}
    COMPATIBILITY AnyNewerVersion)
    
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/chessgen.pc.in"
    "${pkgconfig}"
    @ONLY)

  configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/chessgen-config.cmake.in
    ${project_config}
    INSTALL_DESTINATION ${CHESSGEN_CMAKE_DIR})

  # Use a namespace because CMake provides better diagnostics for namespaced
  # imported targets.
  export(TARGETS ${INSTALL_TARGETS}
         FILE ${PROJECT_BINARY_DIR}/${targets_export_name}.cmake)

  # Install version, config and target files.
  install(FILES ${project_config} ${version_config}
          DESTINATION ${CHESSGEN_CMAKE_DIR})

  install(EXPORT ${targets_export_name} 
          DESTINATION ${CHESSGEN_CMAKE_DIR}
          NAMESPACE chessgen::)

  # Install the library and headers.
  install(TARGETS ${INSTALL_TARGETS} 
          EXPORT ${targets_export_name}
          DESTINATION ${CHESSGEN_LIB_DIR})

  install(FILES ${CHESSGEN_HEADERS} DESTINATION ${CHESSGEN_INC_DIR})
  install(FILES "${pkgconfig}" DESTINATION "${CHESSGEN_PKGCONFIG_DIR}")
endif ()

if(CHESSGEN_TESTS)
  enable_testing()
  add_subdirectory(test)
endif ()


