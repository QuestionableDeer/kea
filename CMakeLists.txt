cmake_minimum_required(VERSION 3.26)

if (NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(
	"kea" 
	VERSION 0.0.1 
	DESCRIPTION "A Game Boy emulator in C++"
	LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR 
		"In-source builds not allowed. Please make a build directory and run there."
		)
endif()

include(ProjectOptions.cmake)

kea_setup_options()
kea_global_options()
kea_local_options()

target_compile_features(kea_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(kea::kea_options ALIAS kea_options)
add_library(kea::kea_warnings ALIAS kea_warnings)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/memory.cpp src/registers.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE kea_options kea_warnings)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

set(test_sources test/test_bits.cpp;test/test_memory.cpp;test/test_registers.cpp)
set(test_helpers src/memory.cpp;src/registers.cpp)

include(cmake/BoostTestHelpers.cmake)
foreach(TEST_NAME ${test_sources})
	add_boost_test(${TEST_NAME} ${test_helpers})
endforeach()
