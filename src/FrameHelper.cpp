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
	    std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to rgb with different data depths. Conversion is not implemented.");
	  else
	    dst.init(src,true);
	break;
	
	//RGB --> grayscale
	case MODE_GRAYSCALE:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to grayscale. Conversion is not implemented.");
	  break;
	  
	//RGB --> bayer pattern  
	case MODE_BAYER:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bayer pattern. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
	  break;
	  
	case MODE_BAYER_RGGB:
	case MODE_BAYER_GRBG:
	case MODE_BAYER_BGGR:
	case MODE_BAYER_GBRG:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bayer pattern. Conversion is not implemented.");
	  break;
	  
	default:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode - mode is unknown");
	}
      break;
    
    //grayscale --> ?
    case MODE_GRAYSCALE:
      std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode grayscale to ?. Conversion is not implemented.");
      break;
    
    //bayer pattern --> ?  
    case MODE_BAYER:
      std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to ?. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
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
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to grayscale. Conversion is not implemented.");
	  break;
	  
	//bayer --> bayer pattern  
	case MODE_BAYER:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to bayer pattern. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
	  break;
	  
	case MODE_BAYER_RGGB:
	case MODE_BAYER_GRBG:
	case MODE_BAYER_BGGR:
	case MODE_BAYER_GBRG:
	  if(src.getDataDepth() == dst.getDataDepth() && dst.getFrameMode() == src.getFrameMode())
	     dst.init(src,true);
	  else
	    std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to bayer pattern with different pattern or data depth. Conversion is not implemented.");
	  break;
	  
	default:
	  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bayer to ? - mode is unknown");
	}
      break;
      
    default:
      std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode ?- mode is unknown");
  }
}
  
  
  
void FrameHelper::convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height, frame_mode_t mode)
{
    const int srcStep = width;
    const int dstStep = 3 * width;
    int blue = mode == MODE_BAYER_RGGB
	      || mode == MODE_BAYER_GRBG ? -1 : 1;
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