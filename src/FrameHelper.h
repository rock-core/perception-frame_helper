#ifndef FRAMEHELPER_H
#define FRAMEHELPER_H

#include <cv.h>
#include "base/samples/frame.h"
#include "FrameHelperTypes.h"

//TODO 
//at the moment some functions call copyImageIndependantAttributes
//automatically
//this should be replaced by a flag

class FrameHelper
{
  private:
    //the buffer are used to convert one frame to an other one 
    //if src and dst attributes are not changing no memory allocation
    //is done
    base::samples::frame::Frame frame_buffer;
    base::samples::frame::Frame frame_buffer2;
    cv::Mat mat1;
    cv::Mat mat2;

  public:
    FrameHelper(){};

    //converts one frame to another frame
    //this can be used to convert colors resize and undistort frames
    //the attributes are implied by the dst attributes
    //call setCalibrationParameters to set the calibration parameters
    //this is none static because an internal buffer is used which is not be resized if the
    //src and dst have always the same attributes
    void convert(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,
                 int offset_x = 0, int offset_y = 0, ResizeAlgorithm algo = INTER_LINEAR,bool undisort=false);
    void setCalibrationParameters(const CalibrationParameters &para)
    {calcCalibrationMatrix(para,mat1,mat2);};
    void setCalibrationParameters(const cv::Mat &mat1, const cv::Mat &mat2){this->mat1 = mat1; this->mat2 = mat2;};

    void static calcCalibrationMatrix(const CalibrationParameters &para, cv::Mat &mat1, cv::Mat &mat2)
    {};
    void static calcStereoCalibrationMatrix(const CalibrationParameters &para1,
                                            const CalibrationParameters &para2,
                                            cv::Mat &mat11, cv::Mat &mat21,
                                            cv::Mat &mat12, cv::Mat &mat22);

    //this is a convenience function to undistort a frame
    //see cv::remap 
    static void undistort(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,
                          const cv::Mat &mat1,const cv::Mat &mat2);

    //resizes a bayer image without converting it to another color format by skipping pixels
    //The size of src must be a multiple of two times dst size
    //otherwise an exception will be thrown
    static void resize_bayer(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,
                             int offset_x = 0, int offset_y = 0);

    //resize a frame
    //the size is implied by the dst frame
    static void resize(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,
                       int offset_x = 0, int offset_y = 0,ResizeAlgorithm algo = INTER_LINEAR);

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
