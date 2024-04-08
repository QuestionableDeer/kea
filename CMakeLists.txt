cmake_minimum_required(VERSION 3.26)

project("kea" VERSION 0.0.1 LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR 
		"In-source builds not allowed. Please make a build directory and run there."
		)
endif()

set(exe_sources src/main.cpp)
set(headers src/bits.h src/types.h)

add_executable(${PROJECT_NAME} ${exe_sources})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
