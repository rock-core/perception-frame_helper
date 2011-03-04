#ifndef FRAMEHELPER_H
#define FRAMEHELPER_H

#include "base/samples/frame.h"

#ifdef __OPENCV_ALL_HPP__
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
    static void convertColor(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst);
    static void convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height ,base::samples::frame::frame_mode_t mode);
    static void convertRGBToGray(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,bool copy_attributes =true);

    static void convertBayerToGreenChannel(const uint8_t *src, uint8_t *dst, int width, int height, base::samples::frame::frame_mode_t mode);
    static void convertBayerToGreenChannel(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst);
};

#endif // FRAMEHELPER_H
