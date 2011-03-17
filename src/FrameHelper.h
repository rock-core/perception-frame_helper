#ifndef FRAMEHELPER_H
#define FRAMEHELPER_H

#include "base/samples/frame.h"

#ifdef __OPENCV_ALL_HPP__

//structure for camera calibration 
//this is only included if opencv is present 
struct CalibrationParameters
{
        double fx1, fy1, cx1, cy1, d01, d11, d21, d31;
        double fx2, fy2, cx2, cy2, d02, d12, d22, d32;
        double tx, ty, tz;
        double rx, ry, rz;
        cv::Mat cameraMatrix1, cameraMatrix2;
        cv::Mat distCoeffs1, distCoeffs2;
        cv::Mat R, T, Q;
        cv::Mat map11, map21, map12, map22;
};
#endif

class FrameHelper
{
  public:
    FrameHelper(){};

    //converts src to dst 
    //the mode of dst implies the conversion 
    static void convertColor(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst);

    //converts a bayer pattern image to rgb image which has 8 bit data depth for each channel
    static void convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height ,base::samples::frame::frame_mode_t mode);

    //converts a rgb image to gray scale image
    static void convertRGBToGray(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,bool copy_attributes =true);

    //copies only the green channel of a bayer pattern image to dst
    //dst has to be MODE_GRAYSCALE
    static void convertBayerToGreenChannel(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst);
    static void convertBayerToGreenChannel(const uint8_t *src, uint8_t *dst, int width, int height, base::samples::frame::frame_mode_t mode);

    //calculates the diff between src1 and src2 
    //dst[i] = |src1[i]-src2[i]|
    //only 8 Bit data depth is supported 
    static void calcDiff(const base::samples::frame::Frame &src1,const base::samples::frame::Frame &src2,base::samples::frame::Frame &dst);
};

#endif // FRAMEHELPER_H
