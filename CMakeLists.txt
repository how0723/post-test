# set minimum cmake version
cmake_minimum_required(VERSION 3.0)
project(network-test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set (CHTX_PR_ALG_VERSION_MAJOR 1)
set (CHTX_PR_ALG_VERSION_MINOR 0)

# c++11 required
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_HOST_UNIX)
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )
endif(CMAKE_HOST_UNIX)

option (USE_OPENCV "USE OPENCV LIB" ON) 

if (USE_OPENCV)
	if(CMAKE_HOST_WIN32)
		set(OPENCV_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/opencv/include)
		set(OPENCV_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/opencv/lib/opencv_world341.lib )
		include_directories(${OPENCV_INCLUDE_DIRS})
		link_libraries(${OPENCV_LIBS})
	else()
		find_package(OpenCV REQUIRED)
		include_directories(${OpenCV_INCLUDE_DIRS})
		link_libraries(${OpenCV_LIBS})
	endif()	
endif (USE_OPENCV)

if(CMAKE_HOST_WIN32)
	ADD_DEFINITIONS(-DWIN32)
	ADD_DEFINITIONS(-D_WINDOWS)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
endif(CMAKE_HOST_WIN32)	

include_directories(include)

set(POST_TEST_SOURCE_FILES 
	src/post-test/main.cpp
	src/log4z/log4z.cpp 
)
set(POST_TEST_HEADER_FILES
	include/httplib.h
	src/log4z/log4z.h 
)

if(CMAKE_HOST_WIN32)
	link_libraries(ws2_32)
endif(CMAKE_HOST_WIN32)	

add_executable("post-test" ${POST_TEST_SOURCE_FILES} ${POST_TEST_HEADER_FILES} )
