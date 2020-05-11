# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.

cmake_minimum_required(VERSION 3.1)
project(frame_helper
        VERSION 1.0
        DESCRIPTION "Library for manipulating Rock Core image type")
find_package(Rock)
rock_init()

option(OPENCV4 "Whether OpenCV 4 or later should be used" OFF)
if (OPENCV4)
    set(opencv_pkgconfig_suffix 4)
endif()

rock_standard_layout()
set(PROJECT_DESCRIPTION "Library for manipulating frames")
