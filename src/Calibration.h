#ifndef __FRAME_HELPER_CALIBRATION_H__
#define __FRAME_HELPER_CALIBRATION_H__

#include <string>
#include <Eigen/Geometry>
#include <base/Eigen.hpp>
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
	    width(-1), height(-1), pixel_error(base::NaN<double>()*base::Vector2d::Ones())
	{}

	CameraCalibration( double fx, double fy, double cx, double cy, double d0, double d1, double d2, double d3 )
	    : fx(fx), fy(fy), cx(cx), cy(cy), 
	    d0(d0), d1(d1), d2(d2), d3(d3),
	    width(-1), height(-1), pixel_error(base::NaN<double>()*base::Vector2d::Ones())
	{}

	double fx, fy, cx, cy, d0, d1, d2, d3;
        int width, height;

        /** Calibration Errors 1-sigma standard deviation **/
        base::Vector2d pixel_error; /** Re-projection error in pixels during calibration (x and y coordinates) */

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
	    c.d0 = frame.getAttribute<double>("d0");
	    c.d1 = frame.getAttribute<double>("d1");
	    c.d2 = frame.getAttribute<double>("d2");
	    c.d3 = frame.getAttribute<double>("d3");
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
	void toFrame( base::samples::frame::Frame& frame ) const
	{
	    frame.setAttribute<double>("fx", fx);
	    frame.setAttribute<double>("fy", fy);
	    frame.setAttribute<double>("cx", cx);
	    frame.setAttribute<double>("cy", cy);
	    frame.setAttribute<double>("d0", d0);
	    frame.setAttribute<double>("d1", d1);
	    frame.setAttribute<double>("d2", d2);
	    frame.setAttribute<double>("d3", d3);

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

	ExtrinsicCalibration()
            : tx(base::unset<double>()), ty(base::unset<double>()), tz(base::unset<double>()), 
              rx(base::unset<double>()), ry(base::unset<double>()), rz(base::unset<double>())
        {}

	ExtrinsicCalibration( double tx, double ty, double tz, double rx, double ry, double rz )
            : tx(tx), ty(ty), tz(tz), 
              rx(rx), ry(ry), rz(rz)
        {}

	/**
	 * @return true if the calibration values are set
	 */
	bool isValid() const
	{
	    return 
		!base::isUnset<double>(tx) &&	
		!base::isUnset<double>(ty) &&	
		!base::isUnset<double>(tz) &&	
		!base::isUnset<double>(rx) &&	
		!base::isUnset<double>(ry) &&	
		!base::isUnset<double>(rz);
	}

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
            target = *this;
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
	 * @return true if the calibration values are set
	 */
	bool isValid() const
	{
            return
                camLeft.isValid() &&
                camRight.isValid() &&
                extrinsic.isValid();
        }

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
