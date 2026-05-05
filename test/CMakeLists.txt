cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(FrogIT CONFIG REQUIRED) # for FrogITApp, project_options, ...

  if(NOT TARGET FrogIT_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# Collect FrogIT source files
file(GLOB_RECURSE FROGIT_SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/FrogITApp/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/FrogITApp/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/FrogITApp/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/FrogITApp/*.h"
)

# Remove Main.cpp as it contains a main() function that conflicts with tests
list(FILTER FROGIT_SRC EXCLUDE REGEX "Main\\.cpp$")

add_executable(tests tests.cpp ${FROGIT_SRC})
target_link_libraries(
  tests
  PRIVATE FrogIT::FrogIT_warnings
          FrogIT::FrogIT_options
     Catch2::Catch2WithMain
          SFML::Graphics 
          SFML::Audio
        ImGui-SFML::ImGui-SFML
          fmt::fmt)

target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src/FrogITApp")
target_include_directories(tests PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

# Copy assets next to the tests binary
add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:tests>/assets
)

if(APPLE)
  target_link_libraries(tests PRIVATE "-framework Cocoa")
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(tests PRIVATE X11 Xext)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml
  WORKING_DIRECTORY 
  $<TARGET_FILE_DIR:tests>)

# Add a file containing a set of constexpr tests
add_executable(constexpr_tests constexpr_tests.cpp)
target_link_libraries(
  constexpr_tests
  PRIVATE FrogIT::FrogIT_warnings
          FrogIT::FrogIT_options
          Catch2::Catch2WithMain)

catch_discover_tests(
  constexpr_tests
  TEST_PREFIX
  "constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "constexpr."
  OUTPUT_SUFFIX
  .xml)

# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
target_link_libraries(
  relaxed_constexpr_tests
  PRIVATE FrogIT::FrogIT_warnings
          FrogIT::FrogIT_options
          Catch2::Catch2WithMain)
target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(
  relaxed_constexpr_tests
  TEST_PREFIX
  "relaxed_constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "relaxed_constexpr."
  OUTPUT_SUFFIX
  .xml)
