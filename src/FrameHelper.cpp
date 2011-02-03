#include "FrameHelper.h"
#include "stdexcept"

using namespace base::samples::frame;

void FrameHelper::convertColor(const Frame &src,Frame &dst)
{
    switch(src.getFrameMode())
    {
    case MODE_RGB:
        switch(dst.getFrameMode())
        {
            //RGB --> RGB
        case MODE_RGB:
            if(src.getDataDepth() != dst.getDataDepth())
                throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to rgb with different data depths. Conversion is not implemented.");
            else
                dst.init(src,true);
            break;

            //RGB --> grayscale
        case MODE_GRAYSCALE:
            convertRGBToGray(src,dst);
            break;

            //RGB --> bayer pattern  
        case MODE_BAYER:
            throw  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bayer pattern. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
            break;

        case MODE_BAYER_RGGB:
        case MODE_BAYER_GRBG:
        case MODE_BAYER_BGGR:
        case MODE_BAYER_GBRG:
            throw  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bayer pattern. Conversion is not implemented.");
            break;

        default:
            throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode - mode is unknown");
        }
        break;

        //grayscale --> ?
    case MODE_GRAYSCALE:
        throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode grayscale to ?. Conversion is not implemented.");
        break;

        //bayer pattern --> ?  
    case MODE_BAYER:
        throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to ?. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
        break;

        //bayer pattern --> ?    
    case MODE_BAYER_RGGB:
    case MODE_BAYER_GRBG:
    case MODE_BAYER_BGGR:
    case MODE_BAYER_GBRG:
        switch(dst.getFrameMode())
        {
            //bayer --> RGB
        case MODE_RGB:
            if(src.getDataDepth() != dst.getDataDepth())
                std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to rgb with different data depths. Conversion is not implemented.");
            dst.init(src.getWidth(),src.getHeight(),src.getDataDepth(),dst.getFrameMode());
            convertBayerToRGB24(src.getImageConstPtr(),dst.getImagePtr(),src.getWidth(),src.getHeight(),src.frame_mode);	
            dst.copyImageIndependantAttributes(src);
            break;

            //bayer --> grayscale
        case MODE_GRAYSCALE:
            throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to grayscale. Conversion is not implemented.");
            break;

            //bayer --> bayer pattern  
        case MODE_BAYER:
            throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to bayer pattern. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
            break;

        case MODE_BAYER_RGGB:
        case MODE_BAYER_GRBG:
        case MODE_BAYER_BGGR:
        case MODE_BAYER_GBRG:
            if(src.getDataDepth() == dst.getDataDepth() && dst.getFrameMode() == src.getFrameMode())
                dst.init(src,true);
            else
                throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to bayer pattern with different pattern or data depth. Conversion is not implemented.");
            break;

        default:
            throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to ? - mode is unknown");
        }
        break;

    default:
        throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode ?- mode is unknown");
    }
}

//only a data depth of 1 Byte is supported
void FrameHelper::convertRGBToGray(const Frame &src,Frame &dst,bool copy_attributes)
{
    if(src.getPixelSize() != 3)
        throw std::runtime_error("FrameHelper::convertRGBToGray: Can only convert frame mode rgb 24 bit to grayscale 8 bit!");

    static bool initialized = false;
    static uint8_t rFact[256];
    static uint8_t gFact[256];
    static uint8_t bFact[256];

    if(!initialized)
    {
        // populating some lookup tables
        int i;
        for (i = 0; i < 256; ++i)
        {
            rFact[i] =  (uint8_t)((double)(0.299) * (double)(i));
            gFact[i] =  (uint8_t)((double)(0.587) * (double)(i));
            bFact[i] =  (uint8_t)((double)(0.114) * (double)(i));
        }
        initialized = true;
    }

    //set source to right format if it is not set
    if(src.getWidth() != dst.getWidth() ||
            src.getHeight() != dst.getHeight() ||
            dst.getFrameMode() != MODE_GRAYSCALE ||
            dst.getPixelSize() != 1)
    {
        dst.init(src.getWidth(),src.getHeight(),8,MODE_GRAYSCALE,-1);
    }

    //convert pixels
    const uint8_t *psrc = src.getImageConstPtr();
    uint8_t *pdst = dst.getImagePtr();
    const uint8_t *pend =  src.getLastConstByte();
    ++pend;
    while (psrc != pend)
    {
        uint8_t r = *(psrc++);
        uint8_t g = *(psrc++);
        uint8_t b = *(psrc++);
        uint32_t val =  (uint8_t)(rFact[r] + gFact[g] + bFact[b]);
        *(pdst++) = (uint8_t)( (val > 255) ? 255 : val);
    }

    //copy frame attributes
    if(copy_attributes)
        dst.copyImageIndependantAttributes(src);
}

void FrameHelper::convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height, frame_mode_t mode)
{
    const int srcStep = width;
    const int dstStep = 3 * width;
    int blue = mode == MODE_BAYER_RGGB
        || mode == MODE_BAYER_GRBG ? 1 : -1;
    int start_with_green = mode == MODE_BAYER_GBRG
        || mode == MODE_BAYER_GRBG ;
    int i, imax, iinc;

    if (!(mode==MODE_BAYER_RGGB||mode==MODE_BAYER_GBRG||mode==MODE_BAYER_GRBG||mode==MODE_BAYER_BGGR))
        throw std::runtime_error("Helper::convertBayerToRGB24: Unknown Bayer pattern");

    // add a black border around the image
    imax = width * height * 3;
    // black border at bottom
    for (i = width * (height - 1) * 3; i < imax; i++) 
        dst[i] = 0;

    iinc = (width - 1) * 3;
    // black border at right side
    for (i = iinc; i < imax; i += iinc) {
        dst[i++] = 0;
        dst[i++] = 0;
        dst[i++] = 0;
    }

    dst ++;
    width --;
    height --;

    //for each row 
    for (; height--; src += srcStep, dst += dstStep) {
        const uint8_t *srcEnd = src + width;

        if (start_with_green) {
            dst[-blue] = src[1];
            dst[0] = (src[0] + src[srcStep + 1] + 1) >> 1;
            dst[blue] = src[srcStep];
            src++;
            dst += 3;
        }

        if (blue > 0) {
            for (; src <= srcEnd - 2; src += 2, dst += 6) {
                dst[-1] = src[0];
                dst[0] = (src[1] + src[srcStep] + 1) >> 1;
                dst[1] = src[srcStep + 1];

                dst[2] = src[2];
                dst[3] = (src[1] + src[srcStep + 2] + 1) >> 1;
                dst[4] = src[srcStep + 1];
            }
        } else {
            for (; src <= srcEnd - 2; src += 2, dst += 6) {
                dst[1] = src[0];
                dst[0] = (src[1] + src[srcStep] + 1) >> 1;
                dst[-1] = src[srcStep + 1];

                dst[4] = src[2];
                dst[3] = (src[1] + src[srcStep + 2] + 1) >> 1;
                dst[2] = src[srcStep + 1];
            }
        }

        if (src < srcEnd) {
            dst[-blue] = src[0];
            dst[0] = (src[1] + src[srcStep] + 1) >> 1;
            dst[blue] = src[srcStep + 1];
            src++;
            dst += 3;
        }

        src -= width;
        dst -= width * 3;

        blue = -blue;
        start_with_green = !start_with_green;
    }
}

void FrameHelper::convertBayerToGreenChannel(const Frame &src,Frame &dst)
{

    //check dst format 
    if(dst.frame_mode != MODE_GRAYSCALE)
        std::runtime_error("FrameHelper::converBayerToGreenChannel: Cannot convert frame mode bayer to green channel. Dst image must be of mode MODE_GRAYSCALE.");

    if(src.getDataDepth() != dst.getDataDepth())
        std::runtime_error("FrameHelper::converBayerToGreenChannel: Cannot convert frame mode bayer to green channel. src and dst must have the same data depth.");

    dst.init(src.getWidth(),src.getHeight(),src.getDataDepth(),dst.getFrameMode());
    convertBayerToGreenChannel(src.getImageConstPtr(),dst.getImagePtr(),src.getWidth(),src.getHeight(),src.frame_mode);	
    dst.copyImageIndependantAttributes(src);
}


//reads the green channel from an image which uses a bayer pattern
void FrameHelper::convertBayerToGreenChannel(const uint8_t *src, uint8_t *dst, int width, int height, frame_mode_t mode)
{
    const int srcStep = width;
    const int dstStep =  width;
    int blue = mode == MODE_BAYER_RGGB
        || mode == MODE_BAYER_GRBG ? 1 : -1;
    int start_with_green = mode == MODE_BAYER_GBRG
        || mode == MODE_BAYER_GRBG ;
    int i, imax;

    if (!(mode==MODE_BAYER_RGGB||mode==MODE_BAYER_GBRG||mode==MODE_BAYER_GRBG||mode==MODE_BAYER_BGGR))
        throw std::runtime_error("Helper::convertBayerToRGB24: Unknown Bayer pattern");

    // add a black border around the image
    imax = width * height;
    
    // black border at bottom
    for(i = width * (height - 1); i < imax; i++) 
    {
        dst[i] = 0;
    }

    // black border at right side
    for (i = width-1; i < imax; i += width) 
        dst[i] = 0;

    //   dst ++;
    width --;
    height --;

    //for each row 
    for (; height--; src += srcStep, dst += dstStep) {
        const uint8_t *srcEnd = src + width;
        if (start_with_green) {
            dst[0] = (src[0] + src[srcStep + 1] + 1) >> 1;
            src++;
            dst++;
        }
        if (blue > 0) {
            for (; src <= srcEnd - 2; src += 2, dst += 2) {
                dst[0] = (src[1] + src[srcStep] + 1) >> 1;
                dst[1] = (src[1] + src[srcStep + 2] + 1) >> 1;
            }
        } else {
            for (; src <= srcEnd - 2; src += 2, dst += 2) {
                dst[0] = (src[1] + src[srcStep] + 1) >> 1;
                dst[1] = (src[1] + src[srcStep + 2] + 1) >> 1;
            }
        }

        if (src < srcEnd) {
            dst[0] = (src[1] + src[srcStep] + 1) >> 1;
            src++;
            dst++;
        }
        src -= width;
        dst -= width;

        blue = -blue;
        start_with_green = !start_with_green;
    }
}
