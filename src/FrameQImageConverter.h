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
#include <math.h>
#include <limits>

static const float MAX_H = 10.0f;    //this value is used to convert a 64Bit grayscale image 
//to a rgb image
//if a vaule of 10 is reached ==>  h = 360° but s will be reduced 
namespace frame_helper
{
    class FrameQImageConverter
    {
        public:
            FrameQImageConverter(){};
            virtual
                ~FrameQImageConverter(){};

            /**
             * Converts an HSV color value to RGB. Conversion formula
             * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
             * Assumes h, s, and v are contained in the set [0, 1] and
             * returns r, g, and b in the set [0, 255].
             *
             * @param   Number  h       The hue
             * @param   Number  s       The saturation
             * @param   Number  v       The value
             * @return  Array           The RGB representation
             */
            void hsvToRgb(float h, float s, float v,int &r, int &g, int &b)
            {
                int i = h * 6;
                float f = h * 6 - i;
                float p = v * (1 - s);
                float q = v * (1 - f * s);
                float t = v * (1 - (1 - f) * s);
                switch(i % 6){
                case 0: r = v*255; g = t*255; b = p*255; break;
                case 1: r = q*255; g = v*255; b = p*255; break;
                case 2: r = p*255; g = v*255; b = t*255; break;
                case 3: r = p*255; g = q*255; b = v*255; break;
                case 4: r = t*255; g = p*255; b = v*255; break;
                case 5: r = v*255; g = p*255; b = q*255; break;
                }
            }

            //returns 1 if the size or format of dst has been changed  otherwise 0
            //int copyFrameToQImageRGB888(QImage &dst,base::samples::frame::frame_mode_t mode,int pixel_size, int width,int height,const char* pbuffer)
            //{
            int copyFrameToQImageRGB888(QImage &dst,base::samples::frame::frame_mode_t mode,int pixel_size, unsigned int width,unsigned int height, const char* pbuffer)
            {
                int ireturn = 0;
                //check if dst has the right format
                if((unsigned int) dst.width() != width || (unsigned int) dst.height()!= height || dst.format() != QImage::Format_RGB888)
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
                    // Provide the buffer as const uchar* and call bits() to make QImage
                    // do a deep copy. This is needed to ensure that QImage gets proper
                    // buffer alignment
                    dst = QImage((const uchar*)pbuffer, width, height, width*pixel_size, QImage::Format_RGB888);
                    dst.bits();
                    break;

                case base::samples::frame::MODE_UYVY:
                {
                    // WARNING
                    // WARNING: this code has been changed in order to account for the
                    // WARNING: 4-byte line padding requirements of QImage
                    // WARNING: due to the lack of an UYVY image provider at the moment,
                    // WARNING: it cannot be tested
                    // WARNING
                    // WARNING: please contact rock-dev if it does not work.
                    unsigned int i,j;
                    uint8_t u,v,y1,y2,cb,cr,r1,r2,b1,b2,g1,g2;
                    for(i = 0 ;i < height ;++i){
                        uint8_t* output_line = dst.scanLine(i);
                        for(j = 0 ;j < width ;j+=2){
                            u      = pbuffer[(i*width*2)+j*2+0];
                            y1     = pbuffer[(i*width*2)+j*2+1];
                            v      = pbuffer[(i*width*2)+j*2+2];
                            y2      = pbuffer[(i*width*2)+j*2+3];

                            cb = u;
                            cr = v;	

                            //no-rounding conversion method
                            r1 = (255/219)*(y1-16) + (255/112)*0.701*(cr-128);
                            g1 = (255/219)*(y1-16) + (255/112)*0.886*(0.114/0.587)*(cb-128)-(255/112)*0.701*(0.299/0.587)*(cr-128);
                            b1 = (255/219)*(y1-16) + (255/112)*0.886*(cb-128);

                            r2 = (255/219)*(y2-16) + (255/112)*0.701*(cr-128);
                            g2 = (255/219)*(y2-16) + (255/112)*0.886*(0.114/0.587)*(cb-128)-(255/112)*0.701*(0.299/0.587)*(cr-128);
                            b2 = (255/219)*(y2-16) + (255/112)*0.886*(cb-128);

                            output_line[j*3+0] = r1;
                            output_line[j*3+1] = g1;
                            output_line[j*3+2] = b1;
                            output_line[j*3+3] = r2;
                            output_line[j*3+4] = g2;
                            output_line[j*3+5] = b2;
                        }
                    }
                    break;
                }
                case base::samples::frame::MODE_GRAYSCALE:
                {
                    switch(pixel_size)
                    {
                        case 1:
                            dst = QImage((uchar*)pbuffer, width, height, width, QImage::Format_Indexed8).convertToFormat(QImage::Format_RGB888);
                            break;
                        case 8:     //we have to scale the 64 data depth --> a rgb color coding is used
                        {
                            static const float SCALE_H = 1/MAX_H;
                            static const double SCALE_S = 1/floor(std::numeric_limits<double>::max()*SCALE_H);

                            int pixels = width*height;
                            double *data = (double*)pbuffer;
                            unsigned char* data2 =  dst.bits();
                            int r,g,b;
                            float h,s,v;
                            v = 1;      //use always a value of one for the color coding
                            //h and s are changed accordingly to the pixel value

                            //for each pixel of the map 
                            for(int i=0;i < pixels;++i,++data)
                            {
                                //display black if value is +- infinity
                                if(fabs(*data) == std::numeric_limits<double>::infinity())
                                {
                                    r = 0;
                                    g = 0;
                                    b = 0;
                                }
                                else
                                {
                                    //+ and - values are displayed in the same way
                                    if(*data > 0)
                                    {
                                        h = fmod(*data,MAX_H)*SCALE_H;
                                        s = 1-floor(*data*SCALE_H)*SCALE_S;
                                    }
                                    else
                                    {
                                        h = fmod(-*data,MAX_H)*SCALE_H;
                                        s = 1-floor(-*data*SCALE_H)*SCALE_S;
                                    }
                                    hsvToRgb(h,s,v,r,g,b);
                                }
                                //copy data to dest image
                                *data2 = (unsigned char) r;
                                ++data2;
                                *data2 = (unsigned char) g;
                                ++data2;
                                *data2 = (unsigned char) b;
                                ++data2;
                            }
                            break;
                        }
                        default:
                            throw std::runtime_error("Pixel size is not supported by FrameQImageConverter!");
                    }
                    break;
                }


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

            int copyFrameToQImageRGB888(QImage &dst,const QString &mode,int pixel_size, int width,int height, const char* pbuffer)
            {
                base::samples::frame::frame_mode_t _mode =  base::samples::frame::Frame::toFrameMode(mode.toStdString());
                return copyFrameToQImageRGB888(dst,_mode,pixel_size,width,height,pbuffer);
            };

        private:
            QImage image_buffer;
    };
    };

#endif /* FRAMEQIMAGECONVERTER_H_ */
