
ADD_EXECUTABLE(frame_helper_testsuite test_suite.cpp ../src/FrameHelper.cpp)
TARGET_LINK_LIBRARIES(frame_helper_testsuite ${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES} ${Boost_SYSTEM_LIBRARIES} ${GSL_LIBRARIES} ${OPENCV_LIBRARIES} )

