include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


include(CheckCXXSourceCompiles)


macro(FrogIT_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)

    message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
      set(SUPPORTS_UBSAN ON)
    else()
      message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if (NOT WIN32)
      message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(FrogIT_setup_options)
  option(FrogIT_ENABLE_HARDENING "Enable hardening" ON)
  option(FrogIT_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    FrogIT_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    FrogIT_ENABLE_HARDENING
    OFF)

  FrogIT_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR FrogIT_PACKAGING_MAINTAINER_MODE)
    option(FrogIT_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(FrogIT_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(FrogIT_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(FrogIT_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(FrogIT_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(FrogIT_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(FrogIT_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(FrogIT_ENABLE_PCH "Enable precompiled headers" OFF)
    option(FrogIT_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(FrogIT_ENABLE_IPO "Enable IPO/LTO" ON)
    option(FrogIT_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(FrogIT_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(FrogIT_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(FrogIT_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(FrogIT_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(FrogIT_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(FrogIT_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(FrogIT_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(FrogIT_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(FrogIT_ENABLE_PCH "Enable precompiled headers" OFF)
    option(FrogIT_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      FrogIT_ENABLE_IPO
      FrogIT_WARNINGS_AS_ERRORS
      FrogIT_ENABLE_USER_LINKER
      FrogIT_ENABLE_SANITIZER_ADDRESS
      FrogIT_ENABLE_SANITIZER_LEAK
      FrogIT_ENABLE_SANITIZER_UNDEFINED
      FrogIT_ENABLE_SANITIZER_THREAD
      FrogIT_ENABLE_SANITIZER_MEMORY
      FrogIT_ENABLE_UNITY_BUILD
      FrogIT_ENABLE_CLANG_TIDY
      FrogIT_ENABLE_CPPCHECK
      FrogIT_ENABLE_COVERAGE
      FrogIT_ENABLE_PCH
      FrogIT_ENABLE_CACHE)
  endif()

  FrogIT_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (FrogIT_ENABLE_SANITIZER_ADDRESS OR FrogIT_ENABLE_SANITIZER_THREAD OR FrogIT_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(FrogIT_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(FrogIT_global_options)
  if(FrogIT_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    FrogIT_enable_ipo()
  endif()

  FrogIT_supports_sanitizers()

  if(FrogIT_ENABLE_HARDENING AND FrogIT_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR FrogIT_ENABLE_SANITIZER_UNDEFINED
       OR FrogIT_ENABLE_SANITIZER_ADDRESS
       OR FrogIT_ENABLE_SANITIZER_THREAD
       OR FrogIT_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${FrogIT_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${FrogIT_ENABLE_SANITIZER_UNDEFINED}")
    FrogIT_enable_hardening(FrogIT_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(FrogIT_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(FrogIT_warnings INTERFACE)
  add_library(FrogIT_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  FrogIT_set_project_warnings(
    FrogIT_warnings
    ${FrogIT_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(FrogIT_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    FrogIT_configure_linker(FrogIT_options)
  endif()

  include(cmake/Sanitizers.cmake)
  FrogIT_enable_sanitizers(
    FrogIT_options
    ${FrogIT_ENABLE_SANITIZER_ADDRESS}
    ${FrogIT_ENABLE_SANITIZER_LEAK}
    ${FrogIT_ENABLE_SANITIZER_UNDEFINED}
    ${FrogIT_ENABLE_SANITIZER_THREAD}
    ${FrogIT_ENABLE_SANITIZER_MEMORY})

  set_target_properties(FrogIT_options PROPERTIES UNITY_BUILD ${FrogIT_ENABLE_UNITY_BUILD})

  if(FrogIT_ENABLE_PCH)
    target_precompile_headers(
      FrogIT_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(FrogIT_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    FrogIT_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(FrogIT_ENABLE_CLANG_TIDY)
    FrogIT_enable_clang_tidy(FrogIT_options ${FrogIT_WARNINGS_AS_ERRORS})
  endif()

  if(FrogIT_ENABLE_CPPCHECK)
    FrogIT_enable_cppcheck(${FrogIT_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(FrogIT_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    FrogIT_enable_coverage(FrogIT_options)
  endif()

  if(FrogIT_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(FrogIT_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(FrogIT_ENABLE_HARDENING AND NOT FrogIT_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR FrogIT_ENABLE_SANITIZER_UNDEFINED
       OR FrogIT_ENABLE_SANITIZER_ADDRESS
       OR FrogIT_ENABLE_SANITIZER_THREAD
       OR FrogIT_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    FrogIT_enable_hardening(FrogIT_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
