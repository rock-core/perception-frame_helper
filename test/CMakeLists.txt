rock_testsuite(test
                 SOURCES test_suite.cpp
                 DEPS_PKGCONFIG base-types opencv libv4lconvert libv4l2 jpeg_conversion
                 DEPS frame_helper
                 )
