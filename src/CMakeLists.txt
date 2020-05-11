find_package(Boost REQUIRED COMPONENTS regex)

pkg_check_modules(LIBV4l2 libv4l2)
pkg_check_modules(LIBV4l2C libv4l2convert)
if(LIBV4l2_FOUND AND LIBV4l2C_FOUND)
    add_definitions(-DWITH_LIBV4l2)
    add_definitions(${LIBV4l2_CFLAGS})
    add_definitions(${LIBV4l2C_CFLAGS})
    include_directories(${LIBV4l2_INCLUDE_DIRS})
    include_directories(${LIBV4l2C_INCLUDE_DIRS})
    link_directories(${LIBV4l2_LIBRARY_DIRS})
    link_directories(${LIBV4l2C_LIBRARY_DIRS})
else(LIBV4l2_FOUND AND LIBV4l2C_FOUND)
    MESSAGE("## building without v4l2 ##")
endif(LIBV4l2_FOUND AND LIBV4l2C_FOUND)

if(DEFINED ENV{EXTERNAL_OPENCV_PATH})
    message("EXTERNAL_OPENCV_PATH is set, searching in location: $ENV{EXTERNAL_OPENCV_PATH}")
    find_package(OpenCV REQUIRED PATHS $ENV{EXTERNAL_OPENCV_PATH} NO_CMAKE_SYSTEM_PATH)
else()
    message("OPENCV_INSTALL_DIR not set, searching in default location(s)")
    find_package(OpenCV REQUIRED NO_CMAKE_ENVIRONMENT_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PATH)
endif(DEFINED ENV{EXTERNAL_OPENCV_PATH})


include_directories("${OpenCV_INCLUDE_DIRS}")
link_directories(${OpenCV_LIBRARY_DIRS})

rock_library(${PROJECT_NAME}
    SOURCES FrameHelper.cpp Calibration.cpp CalibrationCv.cpp ColorGradient.cpp
    HEADERS Calibration.h
        CalibrationCv.h
        ColorGradient.h
        ColorGradientTypes.h
        FrameHelper.h
        FrameHelperTypes.h
        FrameQImageConverter.h
    DEPS_PKGCONFIG base-types jpeg_conversion
    DEPS_PLAIN
        Boost_REGEX
    LIBS ${LIBV4l2_LIBRARIES} ${LIBV4l2C_LIBRARIES} ${OpenCV_LIBRARIES}
)
