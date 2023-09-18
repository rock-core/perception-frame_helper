#ifndef BOOST_TEST_DYN_LINK
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "frame_helper"
#define BOOST_AUTO_TEST_MAIN
#include "../src/FrameHelper.h"
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>

#include "../src/Calibration.h"

using namespace std;
using namespace base::samples::frame;
using namespace frame_helper;

bool compareImages(cv::Mat src1, cv::Mat src2)
{
    if (src1.rows != src2.rows) {
        return false;
    }
    if (src1.cols != src2.cols) {
        return false;
    }
    if (src1.channels() != src2.channels()) {
        return false;
    }
    if (src1.data == src2.data) {
        return false;
    }
    return true;
}

BOOST_AUTO_TEST_CASE(convert_image)
{
    std::string const filename = TEST_SRC_DIR + string("/test.jpg");
    // load image
    cv::Mat original = cv::imread(filename);
    BOOST_CHECK(NULL != original.data);

    // check if convertion is working
    Frame frame;
    frame.init(original.cols, original.rows, 8, MODE_BGR, -1);
    memcpy(frame.getImagePtr(),
        original.ptr(),
        original.rows * original.cols * original.elemSize());
    cv::Mat converted = FrameHelper::convertToCvMat(frame);
    BOOST_CHECK(compareImages(original, converted));

    Frame frame_resized(100, 100, 8, MODE_BGR);
    FrameHelper::resize(frame, frame_resized);
    cv::Mat resized = FrameHelper::convertToCvMat(frame_resized);

    cv::FileStorage resized_fs(TEST_SRC_DIR + string("/resized.bin"),
        cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML);
    cv::Mat resized_sheet;
    resized_fs["Image"] >> resized_sheet;
    resized_fs.release();
    BOOST_CHECK(NULL != resized_sheet.data);

    BOOST_CHECK(compareImages(resized_sheet, resized));

    FrameHelper frame_helper;
    frame_helper.convert(frame, frame_resized);
    resized = FrameHelper::convertToCvMat(frame_resized);
    BOOST_CHECK(compareImages(resized_sheet, resized));

    // convert color
    Frame frame_grayscale(300, 300, 8, MODE_GRAYSCALE);
    frame_helper.convert(frame, frame_grayscale);
    cv::Mat gray = FrameHelper::convertToCvMat(frame_grayscale);

    cv::FileStorage grayscale_fs(TEST_SRC_DIR + string("/grayscale.bin"),
        cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML);
    cv::Mat grayscale_sheet;
    grayscale_fs["Image"] >> grayscale_sheet;
    grayscale_fs.release();
    BOOST_CHECK(compareImages(grayscale_sheet, gray));

    Frame frame_jpeg;
    FrameHelper::loadFrameJPEG(filename, frame_jpeg);
    BOOST_CHECK(frame_jpeg.size == frame.size);
    BOOST_CHECK_EQUAL(frame_jpeg.frame_mode, MODE_JPEG);
    frame_helper.convertColor(frame, frame_jpeg);

    Frame frame_jpeg_dataonly;
    FrameHelper::loadFrameJPEG(frame_jpeg.image.data(), frame_jpeg.image.size(), frame_jpeg_dataonly);
    frame_jpeg_dataonly.setImage(frame_jpeg.image);
    BOOST_CHECK(frame_jpeg_dataonly.size == frame.size);
    BOOST_CHECK_EQUAL(frame_jpeg_dataonly.frame_mode, MODE_JPEG);
}

BOOST_AUTO_TEST_CASE(load_calibration_file)
{
    StereoCalibration calib;
    BOOST_CHECK(!calib.isValid());
    calib =
        StereoCalibration::fromMatlabFile(TEST_SRC_DIR + string("/calib.txt"), 320, 240);
    BOOST_CHECK(calib.isValid());
}
