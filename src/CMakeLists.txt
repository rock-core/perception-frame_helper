###### Uncomment the following block for Qt setup
# include_directories(${CMAKE_CURRENT_BINARY_DIR})
# # Add your .ui files ot the following line (and uncomment it)
# QT4_WRAP_UI(ui_cpp myproject.ui)
# # Add the .h files that define QObject subclasses (that have a Q_OBJECT
# # statement)
# QT4_WRAP_CPP(moc_cpp dummy.h)

###### Define a binary
# Note: the library (see below) and executable names can't be the same
#add_executable(${PROJECT_NAME} main.cpp dummy.cpp ${moc_cpp} ${ui_cpp})

###### Define a library
# Note: the library and executable names can't be the same
add_library(${PROJECT_NAME} SHARED FrameHelper.cpp Calibration.cpp CalibrationCv.cpp ${moc_cpp} ${ui_cpp})
# Link the defined target (either binary or library, see above) to libraries.
# You must use ${QT_LIBRARIES} if you have enabled Qt support in the root
# CMakeLists.txt, and ${Boost_LIBRARIES} for boost libraries
#
# If you want to use a library defined in this package, just use its name
#


pkg_check_modules(V4LCONVERT "libv4lconvert" REQUIRED)
include_directories(${V4LCONVERT_INCLUDE_DIRS})
link_directories(${V4LCONVERT_LIBRARY_DIRS})

pkg_check_modules(V4L2 "libv4l2" REQUIRED)
include_directories(${V4L2_INCLUDE_DIRS})
link_directories(${V4L2_LIBRARY_DIRS})

pkg_check_modules(JPEG_CONV "jpeg_conversion" REQUIRED)
include_directories(${JPEG_CONV_INCLUDE_DIRS})
link_directories(${JPEG_CONV_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} ${EXTERNAL_LIBS} ${OPENCV_LIBRARIES} ${V4L2_LIBRARIES} ${V4LCONVERT_LIBRARIES} ${JPEG_CONV_LIBRARIES})

install(TARGETS ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)
