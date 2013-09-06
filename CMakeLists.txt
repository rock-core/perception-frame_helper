# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 2.6)

project(frame_helper)
find_package(Rock)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "Library for manipulating frames")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
include(CommonSetup)

# Include headers within source
# If you create subdirectories within source include headers with subdirectory, i.e. 
# #include "subdirectory/myheader.h"
include_directories(${PROJECT_SOURCE_DIR}/external/include)
include_directories(${PROJECT_SOURCE_DIR}/src)

include(FindPkgConfig)
pkg_check_modules(BASE "base-types" )
include_directories(${BASE_INCLUDE_DIRS})
link_directories(${BASE_LIBDIR})

rock_find_pkgconfig(OPENCV REQUIRED opencv)
include_directories(${OPENCV_INCLUDE_DIRS})
link_directories(${OPENCV_LIBRARY_DIRS})

pkg_check_modules(V4LCONVERT "libv4lconvert" REQUIRED)
include_directories(${V4LCONVERT_INCLUDE_DIRS})
link_directories(${V4LCONVERT_LIBRARY_DIRS})

pkg_check_modules(V4L2 "libv4l2" REQUIRED)
include_directories(${V4L2_INCLUDE_DIRS})
link_directories(${V4L2_LIBRARY_DIRS})

pkg_check_modules(JPEG_CONV "jpeg_conversion" REQUIRED)
include_directories(${JPEG_CONV_INCLUDE_DIRS})
link_directories(${JPEG_CONV_LIBRARY_DIRS})

# command line output on, you can also use make VERBOSE=1
# set(CMAKE_VERBOSE_MAKEFILE on)

### Uncomment if you want to use Qt
# set(QT_SEARCH_PATH /opt)
# find_package(Qt4 4.5 REQUIRED QtTest QtThread QtXml QtNetwork QtGui)

# Definitions to use when compiling code that uses Qt
#add_definitions(${QT_DEFINITIONS})

# Set your desired Qt-Version here
#include_directories(${QT_INCLUDE_DIR})

# Path to a Cmake file that can be included to compile Qt4 applications and libraries
#include(${QT_USE_FILE})


### Uncomment if you want to use Boost. Use the first find_package stanza to
### search for headers only, and the second one if you need binary libraries
#set(Boost_ADDITIONAL_VERSION 1.40 1.41)
find_package(Boost REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS unit_test_framework regex)
if (Boost_UNIT_TEST_FRAMEWORK_FOUND)
  message(STATUS "boost/test found")
else()
  message(STATUS "boost/test not found ... NOT building the test suite")
endif()
### Update external libraries that are available
set(EXTERNAL_LIBS ${Boost_LIBRARIES} ${QT_LIBRARIES})

# Process CMakeLists.txt in the following subdirectory
add_subdirectory(src)
add_subdirectory(test)

add_subdirectory(bindings/ruby)


