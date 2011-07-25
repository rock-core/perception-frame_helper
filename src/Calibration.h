#ifndef __FRAME_HELPER_CALIBRATION_H__
#define __FRAME_HELPER_CALIBRATION_H__

#ifndef __orogen
#include <opencv/cv.h>
#endif

namespace frame_helper
{
    /**
     * Calibration parameters expressing the camera matrix and the coefficients
     * for the lens distortion. See: 
     * http://opencv.willowgarage.com/documentation/camera_calibration_and_3d_reconstruction.html
     * for a description of the values and the model that is used.
     */
    struct CameraCalibration
    {
	double fx, fy, cx, cy, d0, d1, d2, d3;
    };

    /** 
     * extrinsic calibration parameters for a stereo camera setup
     */
    struct ExtrinsicCalibration
    {
	double tx, ty, tz;
	double rx, ry, rz;
    };

    /**
     * Full set of parameters required for a stereo camera calibration.
     */
    struct StereoCameraCalibration
    {
	CameraCalibration camLeft, camRight;
	ExtrinsicCalibration extrinsic;
    };

#ifndef __orogen
    /** 
     * Storage structure for opencv undistort maps. 
     *
     * Use the setCalibration function to calculate camMatrix and distCoeffs cv
     * structures.  Optionally set R and P matrices manually (see
     * cv::initUndistortRectifyMap). Call initCv to initiallize map1 and map2,
     * which can be used for the remap call for undistorting an image.
     */
    struct CameraCalibrationCv 
    {
	CameraCalibrationCv();
	void setCalibration( const CameraCalibration& calib );
	void setImageSize( cv::Size imageSize );
	void initCv();

	const CameraCalibration& getCalibration() const { return calib; }
	cv::Size getImageSize() const { return imageSize; }

	cv::Mat camMatrix, distCoeffs;
	cv::Mat R, P;
	cv::Mat map1, map2;

    protected:
	CameraCalibration calib;
	cv::Size imageSize;
    };

    /** 
     * Storage structure for a stereo camera setup.
     *
     * Similar to CameraCalibrationCv, a call to setCallibration will trigger
     * the generation of the CameraCalibrationCv for left and right, as well as
     * R and T matrices. A call to initCv will generate the Q matrix.
     */
    struct StereoCalibrationCv
    {
	StereoCalibrationCv();
	void setCalibration( const StereoCameraCalibration& stereoCalib );
	void setImageSize( cv::Size size );
	void initCv();

	cv::Size getImageSize() const { return imageSize; }

	CameraCalibrationCv camLeft, camRight;
	cv::Mat R, T, Q;
	
    protected:
	ExtrinsicCalibration extrinsic;
	cv::Size imageSize;
    };
#endif
}

#endif
