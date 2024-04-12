macro(kea_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" 
			OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") 
			AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" 
			OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") 
			AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(kea_setup_options)
  kea_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR kea_PACKAGING_MAINTAINER_MODE)
    option(kea_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(kea_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(kea_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(kea_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(kea_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
  else()
    option(kea_ENABLE_IPO "Enable IPO/LTO" ON)
    option(kea_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    # bug specific to my Ubuntu VM version breaks ASAN support...
    option(kea_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF) #${SUPPORTS_ASAN})
    option(kea_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(kea_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(kea_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(kea_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(kea_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
  endif()
endmacro()

macro(kea_global_options)
  if(kea_ENABLE_IPO)
		include(cmake/IPO.cmake)
    kea_enable_ipo()
  endif()

  kea_supports_sanitizers()
endmacro()

macro(kea_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(kea_warnings INTERFACE)
  add_library(kea_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  kea_set_project_warnings(
    kea_warnings
    ${kea_WARNINGS_AS_ERRORS}
    ""
    ""
    "")

  include(cmake/Sanitizers.cmake)
  kea_enable_sanitizers(
    kea_options
    ${kea_ENABLE_SANITIZER_ADDRESS}
    ${kea_ENABLE_SANITIZER_LEAK}
    ${kea_ENABLE_SANITIZER_UNDEFINED}
    ${kea_ENABLE_SANITIZER_THREAD}
    ${kea_ENABLE_SANITIZER_MEMORY})

  include(cmake/StaticAnalyzers.cmake)

  # FIXME: invalid clang tidy errors...
  #if(kea_ENABLE_CLANG_TIDY)
  #  kea_enable_clang_tidy(kea_options ${kea_WARNINGS_AS_ERRORS})
  #endif()

  if(kea_ENABLE_CPPCHECK)
    kea_enable_cppcheck(${kea_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()
endmacro()
