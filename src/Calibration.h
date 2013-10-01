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
	CameraCalibration()
	    : fx(.0f), fy(.0f), cx(.0f), cy(.0f), 
	    d0(.0f), d1(.0f), d2(.0f), d3(.0f),
	    width(-1), height(-1)
	{}

	CameraCalibration( double fx, double fy, double cx, double cy, double d0, double d1, double d2, double d3 )
	    : fx(fx), fy(fy), cx(cx), cy(cy), 
	    d0(d0), d1(d1), d2(d2), d3(d3),
	    width(-1), height(-1)
	{}

	double fx, fy, cx, cy, d0, d1, d2, d3;
        int width, height;
        /**
         * copy to another CamCalibration structure 
         */
        void copyTo(CameraCalibration &target) const
        {
	    target = *this;
        }
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
        /**
         * copy to another ExtrinsicCalibration structure 
         */
        void copyTo(ExtrinsicCalibration &target) const
        {
          target.tx = tx;
          target.ty = ty;
          target.tz = tz;

          target.rx = rx;
          target.ry = ry;
          target.rz = rz;
        }
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
        /**
         * copy to another StereoCalibration structure 
         */
        void copyTo(StereoCalibration &target) const
        {
          camLeft.copyTo(target.camLeft);
          camRight.copyTo(target.camRight);
          extrinsic.copyTo(target.extrinsic);
        }
    };

}

#endif
