#ifndef __FRAME_HELPER_CALIBRATION_H__
#define __FRAME_HELPER_CALIBRATION_H__

#include <string>
#include <Eigen/Geometry>
#include <base/samples/Frame.hpp>
#include <base/Float.hpp>

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
	    : fx(base::unset<float>()), fy(base::unset<float>()), cx(base::unset<float>()), cy(base::unset<float>()), 
	    d0(base::unset<float>()), d1(base::unset<float>()), d2(base::unset<float>()), d3(base::unset<float>()),
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
	 * @return the 3x3 camera matrix, which converts scene points into screen points
	 */
	Eigen::Matrix3f getCameraMatrix() const
	{
	    Eigen::Matrix3f res;
	    res << fx, 0, cx,
		0, fy, cy,
		0, 0, 1.0f;
	    return res;
	}

	/**
	 * @return true if the calibration values are set
	 */
	bool isValid() const
	{
	    return 
		width > 0 &&
		height > 0 &&
		!base::isUnset<float>(fx) &&	
		!base::isUnset<float>(fy) &&	
		!base::isUnset<float>(cx) &&	
		!base::isUnset<float>(cy) &&	
		!base::isUnset<float>(d0) &&	
		!base::isUnset<float>(d1) &&	
		!base::isUnset<float>(d2) &&	
		!base::isUnset<float>(d3);
	}

        /**
         * copy to another CamCalibration structure 
         */
        void copyTo(CameraCalibration &target) const
        {
	    target = *this;
        }

	/**
	 * @brief create a calibration struct based on the information embedded
	 *        in the frame
	 *
	 * this function will throw if the calibration parameters are not embedded
	 * as attributes in the frame
	 */
	static CameraCalibration fromFrame( const base::samples::frame::Frame& frame )
	{
	    CameraCalibration c;
	    c.fx = frame.getAttribute<double>("fx");
	    c.fy = frame.getAttribute<double>("fy");
	    c.cx = frame.getAttribute<double>("cx");
	    c.cy = frame.getAttribute<double>("cy");
	    c.width = frame.size.width;
	    c.height = frame.size.height;
	    return c;
	}

	/**
	 * @brief write the calibration into the attributes of the frame
	 *
	 * will throw if the size of the calibration does not match the size
	 * of the frame
	 */
	void toFrame( base::samples::frame::Frame& frame )
	{
	    frame.setAttribute<double>("fx", fx);
	    frame.setAttribute<double>("fy", fy);
	    frame.setAttribute<double>("cx", cx);
	    frame.setAttribute<double>("cy", cy);

	    if( frame.size.width != width )
		throw std::runtime_error("frame width does not match calibration");
	    if( frame.size.height != height )
		throw std::runtime_error("frame height does not match calibration");
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
