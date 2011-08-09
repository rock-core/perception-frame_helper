#include "CalibrationCv.h"

using namespace frame_helper;

CameraCalibrationCv::CameraCalibrationCv() : initialized(false) {}

void CameraCalibrationCv::setCalibration( const CameraCalibration& calib )
{
    this->calib = calib;

    camMatrix.create(3, 3, CV_64F);
    camMatrix = 0.0;
    camMatrix.at<double>(2,2) = 1.0;
    camMatrix.at<double>(0,0) = calib.fx;
    camMatrix.at<double>(0,2) = calib.cx;
    camMatrix.at<double>(1,1) = calib.fy;
    camMatrix.at<double>(1,2) = calib.cy;

    distCoeffs.create(1, 4, CV_64F);
    distCoeffs.at<double>(0,0) = calib.d0;
    distCoeffs.at<double>(0,1) = calib.d1;
    distCoeffs.at<double>(0,2) = calib.d2;
    distCoeffs.at<double>(0,3) = calib.d3;
}

void CameraCalibrationCv::setImageSize( cv::Size size )
{
    imageSize = size;
}

void CameraCalibrationCv::initCv()
{
    if( imageSize == cv::Size() )
	throw std::runtime_error("CameraCalibrationCv: image size not set.");

    cv::initUndistortRectifyMap(
	    camMatrix, distCoeffs, 
	    R, P, imageSize, 
	    CV_32FC1, map1, map2 );

    initialized = true;
}

bool CameraCalibrationCv::isInitialized() const
{
    return initialized;
}

void CameraCalibrationCv::undistortAndRectify( const cv::Mat& input, cv::Mat& output )
{
    // undistort/rectify image
    cv::remap(input, output, map1, map2, cv::INTER_CUBIC);
}

StereoCalibrationCv::StereoCalibrationCv() : initialized(false) {}

void StereoCalibrationCv::setCalibration( const StereoCalibration& stereoCalib )
{
    camLeft.setCalibration( stereoCalib.camLeft );
    camRight.setCalibration( stereoCalib.camRight );

    this->extrinsic = stereoCalib.extrinsic;

    T.create(3, 1, CV_64F);
    T.at<double>(0,0) = extrinsic.tx;
    T.at<double>(1,0) = extrinsic.ty;
    T.at<double>(2,0) = extrinsic.tz;

    //convert from rotation vector to rotation matrix
    cv::Mat tempRot;
    tempRot.create(3, 1, CV_64F);
    tempRot.at<double>(0,0) = extrinsic.rx;
    tempRot.at<double>(1,0) = extrinsic.ry;
    tempRot.at<double>(2,0) = extrinsic.rz;
    cv::Rodrigues(tempRot, R);
}

void StereoCalibrationCv::setImageSize( cv::Size size )
{
    camLeft.setImageSize( size );
    camRight.setImageSize( size );
    imageSize = size;
}

void StereoCalibrationCv::initCv()
{
    if( imageSize == cv::Size() )
	throw std::runtime_error("CameraCalibrationCv: image size not set.");

    cv::stereoRectify(
	    camLeft.camMatrix,
	    camLeft.distCoeffs,
	    camRight.camMatrix,
	    camRight.distCoeffs,
	    imageSize,
	    R,
	    T,
	    camLeft.R,
	    camRight.R,
	    camLeft.P,
	    camRight.P,
	    Q);

    camLeft.initCv();
    camRight.initCv();

    initialized = true;
}

bool StereoCalibrationCv::isInitialized() const
{
    return initialized && camLeft.isInitialized() && camRight.isInitialized();
}

