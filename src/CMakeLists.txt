find_package(Boost REQUIRED COMPONENTS regex)

rock_library(${PROJECT_NAME}
                 SOURCES FrameHelper.cpp Calibration.cpp CalibrationCv.cpp
                 HEADERS Calibration.h
                        CalibrationCv.h
                        FrameHelper.h
                        FrameHelperTypes.h
                        FrameQImageConverter.h
                        DEPS_PKGCONFIG base-types libv4lconvert libv4l2 jpeg_conversion opencv
                 DEPS_CMAKE Boost
                 )
