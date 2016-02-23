project(webserver)
cmake_minimum_required(VERSION 2.8)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_library(libev SHARED IMPORTED)



#if(SHARED_LIBEV)
#  find_library(LIBEV_LIBRARY NAMES ev)
#  find_path(LIBEV_INCLUDE_DIR ev.h
#    PATH_SUFFIXES include/ev include
#    ) # Find header
#  find_package_handle_standard_args(libev DEFAULT_MSG LIBEV_LIBRARY LIBEV_INCLUDE_DIR)
#else()
#  add_subdirectory(deps/libev)
#  set(LIBEV_INCLUDE_DIR deps/libev)
#endif()


#set( REQUIRED_LIBRARIES libev)



#find_package(LibXml2 REQUIRED)
#if(NOT LIBXML2_FOUND)
#	message(SEND_ERROR "Failed to find LibXml2")
#	return()
#else()
#	include_directories(${LIBXML2_INCLUDE_DIR})
#endif()




aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(webserver ev)

