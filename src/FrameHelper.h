#ifndef FRAMEHELPER_H
#define FRAMEHELPER_H

#include "base/samples/frame.h"

class FrameHelper
{
  public:
    FrameHelper(){};
    static void convertColor(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst);
    static void convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height ,base::samples::frame::frame_mode_t mode);
    static void convertRGBToGray(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,bool copy_attributes =true);
  
};

#endif // FRAMEHELPER_H
