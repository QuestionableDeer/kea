cmake_minimum_required(VERSION 3.26)

if (NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17)
endif()

project("kea" VERSION 0.0.1 LANGUAGES CXX)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR 
		"In-source builds not allowed. Please make a build directory and run there."
		)
endif()

# test out using cppcheck
find_program(CPPCHECK NAMES cppcheck)
if (CPPCHECK)
	if (CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
		set(CPPCHECK_TEMPLATE "vs")
	else()
		set(CPPCHECK_TEMPLATE "gcc")
	endif()

	set(SUPPRESS_DIR "*:${CMAKE_CURRENT_BINARY_DIR}/_deps/*.h")
	message(STATUS "CPPCHECK_OPTIONS suppress: ${SUPPRESS_DIR}")
	set(CMAKE_CXX_CPPCHECK
		${CPPCHECK}
		--template=${CPPCHECK_TEMPLATE}
		--enable=style,performance,warning,portability
		--inline-suppr
		--suppress=cppcheckError
		--suppress=internalAstError
		--suppress=unmatchedSuppression
		--suppress=passedByValue
		--suppress=syntaxError
		--suppress=preprocessorErrorDirective
		--suppress=inconclusive
		--suppress=${SUPPRESS_DIR}
		--error-exitcode=2)

	if(NOT ${CMAKE_CXX_STANDARD} STREQUAL "")
		set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK} --std=c++${CMAKE_CXX_STANDARD})
	endif()

	message(STATUS "CMAKE_CXX_CPPCHECK equals: ${CMAKE_CXX_CPPCHECK}")

else()
	message(${WARNING_MESSAGE} "cppcheck requested but executable not found")
endif()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(exe_sources src/main.cpp)
set(headers src/bits.h src/types.h)

add_executable(${PROJECT_NAME} ${exe_sources})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

set(test_sources test/test_bits.cpp)

include(cmake/BoostTestHelpers.cmake)
foreach(TEST_NAME ${test_sources})
	add_boost_test(${TEST_NAME})
endforeach()
