/*
 * FrameQImageConverter.h
 *
 *  Created on: Oct 15, 2010
 *      Author: aduda
 */

/*
 * This Class is Qt depended !!!
 * It can be used in Qt projects to convert base::samples::frame::Frame
 * to QImages
 *
 */

#ifndef FRAMEQIMAGECONVERTER_H_
#define FRAMEQIMAGECONVERTER_H_

#include <QtGui/QImage>
#include "FrameHelper.h"
#include <stdexcept>

class FrameQImageConverter
{
public:
  FrameQImageConverter(){};
  virtual
  ~FrameQImageConverter(){};

  //returns 1 if the size or format of dst has been changed  otherwise 0
  int copyFrameToQImageRGB888(QImage &dst,base::samples::frame::frame_mode_t mode,int pixel_size, int width,int height,const char* pbuffer)
  {
    int ireturn = 0;
    //check if dst has the right format
    if(dst.width() != width || dst.height()!= height || dst.format() != QImage::Format_RGB888)
    {
        dst = QImage(width,height,QImage::Format_RGB888);
        ireturn = 1;
    }

    switch(mode)
    {
      case base::samples::frame::MODE_UNDEFINED:
        throw std::runtime_error(" FrameQImageConverter::copyFrameToQImageRGB888: Unknown frame mode!");
        break;

       case base::samples::frame::MODE_BAYER_RGGB:
       case base::samples::frame::MODE_BAYER_GRBG:
       case base::samples::frame::MODE_BAYER_BGGR:
       case base::samples::frame::MODE_BAYER_GBRG:
          FrameHelper::convertBayerToRGB24((const uint8_t*)pbuffer,(uint8_t*) dst.bits(), width, height ,mode);
       break;

       case base::samples::frame::MODE_RGB:
          memcpy(dst.bits(),pbuffer,width*height*pixel_size);
          break;

       case base::samples::frame::MODE_GRAYSCALE:
          //There is no conversion available by FrameHelper use qt
          //conversion --> one additional copy

          //check if buffer has the right format
          if(image_buffer.width() != width || image_buffer.height()!= height || image_buffer.format() != QImage::Format_Indexed8)
          {
             image_buffer = QImage(width,height,QImage::Format_Indexed8);
             for(int i = 0;i<256;++i)
               image_buffer.setColor(i,qRgb(i,i,i));
          }

          memcpy(image_buffer.bits(),pbuffer,width*height*pixel_size);
          dst = image_buffer.convertToFormat(QImage::Format_RGB888);
          break;

       default:
         throw std::runtime_error(" FrameQImageConverter::copyFrameToQImageRGB888: Can not convert frame to RGB888!");
    }
    return ireturn;
  };

  int copyFrameToQImageRGB888(QImage &dst,const base::samples::frame::Frame &frame)
  {
    return copyFrameToQImageRGB888(dst,
                      frame.frame_mode,
                      frame.pixel_size,
                      frame.getWidth(),
                      frame.getHeight(),
                      (const char*)frame.getImageConstPtr());
  };

  int copyFrameToQImageRGB888(QImage &dst,const QString &mode,int pixel_size, int width,int height,const char* pbuffer)
  {
    base::samples::frame::frame_mode_t _mode =  base::samples::frame::Frame::toFrameMode(mode.toStdString());
    return copyFrameToQImageRGB888(dst,_mode,pixel_size,width,height,pbuffer);
  };

private:
  QImage image_buffer;
};

#endif /* FRAMEQIMAGECONVERTER_H_ */
