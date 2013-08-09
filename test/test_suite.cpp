
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "frame_helper"
#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "../src/FrameHelper.h"
#include "../src/BrightnessIndicator.h"
#include <iostream>

#include "../src/Calibration.h"

using namespace base::samples::frame;
using namespace frame_helper;

//test if min max search is working
/*BOOST_AUTO_TEST_CASE(convert_image)
{
    //load image 
    cv::Mat image =  cv::imread("test/test.jpg");
    BOOST_CHECK(NULL != image.data);

    //check if convertion is working
    Frame frame;
    frame.init(image.cols,image.rows,8,MODE_BGR);
    memcpy(frame.getImagePtr(),image.ptr(),image.rows*image.cols * image.elemSize());
    cv::Mat image2 = FrameHelper::convertToCvMat(frame);
    cv::namedWindow("Test",CV_WINDOW_AUTOSIZE);
    cv::imshow("Test",image2);
    cv::waitKey(1000);

    //resize image
    Frame frame2(100,100,8,MODE_BGR);
    FrameHelper::resize(frame,frame2);
    cv::imshow("Test",FrameHelper::convertToCvMat(frame2));
    cv::waitKey(2000);
    
    FrameHelper frame_helper;
    frame_helper.convert(frame,frame2);
    cv::imshow("Test",FrameHelper::convertToCvMat(frame2));
    cv::waitKey(2000);
    
    //convert color
    frame2.init(300,300,8,MODE_GRAYSCALE);
    frame_helper.convert(frame,frame2);
    cv::imshow("Test2",FrameHelper::convertToCvMat(frame2));
    cv::waitKey(2000);

    //convert image 

//    BOOST_CHECK_EQUAL(6,result.size());
//    BOOST_CHECK_EQUAL(0,*result[0]);
//    BOOST_CHECK_EQUAL(5,*result[1]);
//    BOOST_CHECK_EQUAL(0,*result[2]);
//    BOOST_CHECK_EQUAL(0,*result[3]);
//    BOOST_CHECK_EQUAL(6,*result[4]);
//    BOOST_CHECK_EQUAL(0,*result[5]);
}


BOOST_AUTO_TEST_CASE( load_calibration_file )
{
    StereoCalibration calib = StereoCalibration::fromMatlabFile( "test/calib.txt" );
}*/

BOOST_AUTO_TEST_CASE( check_simple_brightness_indicator )
{
	//Construct Test grayscale image
	cv::Mat mat(3,3, CV_8UC1);
	mat = cv::Scalar(128);

	//Check if the average brightness is determinend correctly
	SimpleBrightnessIndicator sbi;
	int val = sbi.getBrightness(mat);
	BOOST_CHECK_EQUAL(128, val);
}

BOOST_AUTO_TEST_CASE( check_weighted_brightness_indicator ) 
{
	//Construct Test grayscale image
	cv::Mat mat(10,10, CV_8UC1);
	mat = cv::Scalar(0);

	//Set the points in the middle of the image to a high brightness value
	cv::Mat(mat, cv::Rect(4,4,2,2)) = cv::Scalar(255);

	std::vector<WeightedRect> regions;
	regions.push_back(WeightedRect(-0.2,-0.2,0.2,0.2,8));
	WeightedBoxesBrightnessIndicator wbi(regions);
	int val = wbi.getBrightness(mat);
	BOOST_CHECK_EQUAL(255, val);

	regions.push_back(WeightedRect(0,0.2,1,1,2));
	wbi.setWeightedRegions(regions);
	int val2 = wbi.getBrightness(mat);
	BOOST_CHECK_EQUAL(204, val2);
}
