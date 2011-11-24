#ifndef __FRAME_HELPER_CALIBRATION_H__
#define __FRAME_HELPER_CALIBRATION_H__

#include <string>
#include <Eigen/Geometry>

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

	/** 
	 * @return the transform that when applied to a point in the left 
	 * image will give the point in the right image.
	 */
	Eigen::Affine3d getTransform() const;
    };

    /**
     * Full set of parameters required for a stereo camera calibration.
     */
    struct StereoCalibration
    {
	CameraCalibration camLeft, camRight;
	ExtrinsicCalibration extrinsic;

	/**
	 * Creates a calibration object from the output .txt file of
	 * the matlab calibration toolbox.
	 */
	static StereoCalibration fromMatlabFile( const std::string& file );
    };

}

#endif
