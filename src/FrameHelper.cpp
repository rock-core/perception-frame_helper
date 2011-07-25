#include "FrameHelper.h"
#include <stdexcept>

using namespace base::samples::frame;

namespace frame_helper
{
    enum ConvertMode{
        COPY = 0,
        RESIZE = 1,
        COLOR = 2,
        UNDISTORT = 4
    };

    FrameHelper::FrameHelper()
    {
	calibration.setImageSize( cv::Size( -1, -1 ) );
    }

    void FrameHelper::convert(const base::samples::frame::Frame &src,
            base::samples::frame::Frame &dst,
            int offset_x,
            int offset_y,
            ResizeAlgorithm algo,
            bool bdewrap)
    {
        //set bayer pattern if not specified
        if(dst.getFrameMode() == MODE_BAYER && src.isBayer())
            dst.frame_mode = src.getFrameMode();

        //find out which mode shall be used
        int mode = COPY;
        if(src.getFrameMode() != dst.getFrameMode() || src.getDataDepth() != dst.getDataDepth())
            mode += COLOR;
        if(src.size.width != dst.size.width || src.size.height != dst.size.height)
            mode += RESIZE;
        if(bdewrap)
            mode += UNDISTORT;

        const cv::Mat cv_src = src.convertToCvMat();
        cv::Mat cv_dst = dst.convertToCvMat();

        //this is needed to prevent 
        //copies 
        switch(mode)
        {
        case COPY:
            dst.init(src,true);
            break;
        case RESIZE:
            resize(src,dst,offset_x,offset_y,algo);
            break;
        case COLOR:
            convertColor(src,dst);
            break;
        case UNDISTORT:
            undistort(src,dst);
            break;
        case RESIZE + COLOR:
            frame_buffer.init(src.getWidth(),src.getHeight(),dst.getDataDepth(),dst.getFrameMode(),false);
            convertColor(src,frame_buffer);
            resize(frame_buffer,dst,offset_x,offset_y,algo);
            break;
        case RESIZE + UNDISTORT:
            frame_buffer2.init(dst,false);
            resize (src,frame_buffer2,offset_x,offset_y,algo);
            undistort(frame_buffer2,dst);
            break;
        case COLOR + UNDISTORT:
            frame_buffer2.init(dst,false);
            convertColor(src,frame_buffer2);
            undistort(frame_buffer2,dst);
            break;
        case RESIZE + COLOR + UNDISTORT:
            frame_buffer.init(src.getWidth(),src.getHeight(),dst.getDataDepth(),dst.getFrameMode(),false);
            convertColor(src,frame_buffer);
            frame_buffer2.init(dst,false);
            resize(frame_buffer,frame_buffer2,offset_x,offset_y,algo);
            undistort(frame_buffer2,dst);
            break;
        }
        dst.copyImageIndependantAttributes(src);
    }

    void FrameHelper::setCalibrationParameter(const CameraCalibration &para)
    {
        //calcCalibrationMatrix is called from dewrap when the image size is known
	calibration.setCalibration( para );
    }

    /*
    void FrameHelper::calcCalibrationMatrix(const CameraCalibration &para,int image_width,int image_height, cv::Mat &map_x, cv::Mat &map_y)
    {
	calibration_parameters.imageSize = cv::Size( image_width, image_height );
	calibration_parameters.initCv();

        cv::Mat intrinsic(3, 3, CV_64F);
        intrinsic = 0.0;
        intrinsic.at<double>(2,2) = 1.0;
        intrinsic.at<double>(0,0) = para.fx;
        intrinsic.at<double>(0,2) = para.cx;
        intrinsic.at<double>(1,1) = para.fy;
        intrinsic.at<double>(1,2) = para.cy;

        cv::Mat distortion(1, 4, CV_64F);
        distortion.at<double>(0,0) = para.d0;
        distortion.at<double>(0,1) = para.d1;
        distortion.at<double>(0,2) = para.d2;
        distortion.at<double>(0,3) = para.d3;

        cv::Size target_size = cv::Size(image_width, image_height);
        cv::Mat mat;
        cv::initUndistortRectifyMap(intrinsic,distortion,mat,intrinsic,target_size,CV_32FC1,map_x,map_y);
    }
    */

    void FrameHelper::undistort(const base::samples::frame::Frame &src,
            base::samples::frame::Frame &dst)
    {
        //check if calibration was set;
        if(calibration.getImageSize().width == -1)
            throw std::runtime_error("FrameHelper::undistort: No calibration map was set!");

        //check if format is supported
        if(src.getFrameMode() != MODE_RGB && src.getFrameMode() != MODE_GRAYSCALE )
            throw std::runtime_error("FrameHelper::undistort: frame mode is not supported!");

        //check if size has changed
        if( src.getHeight() != calibration.getImageSize().height ||
           src.getWidth() != calibration.getImageSize().width )
        {
	    calibration.setImageSize( cv::Size( src.getWidth(), src.getHeight() ) );
	    calibration.initCv();
        }

        dst.init(src,false);
        const cv::Mat cv_src = src.convertToCvMat();
        cv::Mat cv_dst = dst.convertToCvMat();
        remap(cv_src, cv_dst, calibration.map1, calibration.map2, cv::INTER_CUBIC);

        //encode the focal length and center into the frame
        dst.setAttribute("fx",calibration.getCalibration().fx);
        dst.setAttribute("fy",calibration.getCalibration().fy);
        dst.setAttribute("cx",calibration.getCalibration().cx);
        dst.setAttribute("cy",calibration.getCalibration().cy);
    }

    void FrameHelper::resize(const base::samples::frame::Frame &src,
            base::samples::frame::Frame &dst,
            int offset_x,int offset_y,
            ResizeAlgorithm algo)
    {
        //check if both images have the same color format
        if(src.getFrameMode() != dst.getFrameMode())
            throw std::runtime_error("FrameHelper::resize: Cannot resize frame. Dst and src have different frame modes.");

        cv::Mat cv_src = src.convertToCvMat();
        cv::Mat cv_dst = dst.convertToCvMat();
        if(offset_x != 0 || offset_y != 0)
            cv_src = cv::Mat(cv_src,cv::Rect(offset_x,offset_y,cv_dst.cols,cv_dst.rows));

        switch(algo)
        {
        case INTER_LINEAR:
            cv::resize(cv_src, cv_dst, cv::Size(cv_dst.cols, cv_dst.rows), 0, 0, cv::INTER_LINEAR);
            break;
        case INTER_NEAREST:
            cv::resize(cv_src, cv_dst, cv::Size(cv_dst.cols, cv_dst.rows), 0, 0, cv::INTER_NEAREST);
            break;
        case INTER_AREA:
            cv::resize(cv_src, cv_dst, cv::Size(cv_dst.cols, cv_dst.rows), 0, 0, cv::INTER_AREA);
            break;
        case INTER_LANCZOS4:
            cv::resize(cv_src, cv_dst, cv::Size(cv_dst.cols, cv_dst.rows), 0, 0, cv::INTER_LANCZOS4);
            break;
        case INTER_CUBIC:
            cv::resize(cv_src, cv_dst, cv::Size(cv_dst.cols, cv_dst.rows), 0, 0, cv::INTER_CUBIC);
            break;
        case BAYER_RESIZE:
            resize_bayer(src,dst,offset_x,offset_y);
        }
    }

    void FrameHelper::resize_bayer(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,
            int offset_x,int offset_y)
    {
        //check if src mode is bayer
        if(!src.isBayer())
            throw std::runtime_error("FrameHelper::resize_bayer: Cannot resize frame. Src mode is not MODE_BAYER.");

        //check if both images have the same color format
        if(src.getFrameMode() != dst.getFrameMode())
            throw std::runtime_error("FrameHelper::resize_bayer: Cannot resize frame. Dst and src have different frame modes.");

        const uint8_t * psrc = src.getImageConstPtr()+offset_y*src.getWidth();
        uint8_t *pdst = dst.getImagePtr();
        int src_width = src.getWidth()-offset_x;
        int src_height = src.getHeight()-offset_y;
        
        const int resize_factor_x = src_width/ dst.size.width;
        const int resize_factor_y = src_height / dst.size.height;

        //check if dst size is a multiple of two
        if(dst.size.width%2 || dst.size.height%2)
            throw std::runtime_error("FrameHelper::resize_bayer: cannot resize frame. "
                    "The dst size is not a multiple of two");

        //check if src size is a multiple of two times dst size
        if(src_width%dst.size.width ||
           src_height%dst.size.height ||
                resize_factor_x != 2||
                resize_factor_y != 2)
            throw std::runtime_error("FrameHelper::resize_bayer: cannot resize frame. "
                    "Only 0.5 as scale factor is supported");

        //for each row
        for(int i=0; i<src_height;++i)
        {
            psrc += offset_x;
            const uint8_t *pend = psrc+src_width;
            //copy row
            while(psrc < pend)
            {
                *(pdst++) = *(psrc++);
                *(pdst++) = *(psrc++);
                //skip columns
                psrc = psrc + resize_factor_x;
            }
            //skip rows
            if(i%2)
            {
                i += resize_factor_y;
                psrc += resize_factor_y*src.getWidth();
            }
        }
    }

    void FrameHelper::convertColor(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst)
    {
        switch(src.getFrameMode())
        {
        case MODE_BGR:
            switch(dst.getFrameMode())
            {
                //BGR --> RGB
            case MODE_RGB:
                if(src.getDataDepth() != dst.getDataDepth())
                    throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bgr with different data depths. Conversion is not implemented.");
                else
                {
                    const cv::Mat cv_src = src.convertToCvMat();
                    cv::Mat cv_dst = dst.convertToCvMat();
                    cv::cvtColor(cv_src,cv_dst,CV_BGR2RGB);
                }
                break;

                //BGR --> grayscale
            case MODE_GRAYSCALE:
                {
                    const cv::Mat cv_src = src.convertToCvMat();
                    cv::Mat cv_dst = dst.convertToCvMat();
                    cv::cvtColor(cv_src,cv_dst,CV_BGR2GRAY);
                    break;
                }
                //RGB --> bayer pattern  
            case MODE_BAYER:
                throw  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode rgb to bayer pattern. Please specify bayer pattern (RGGB,GRBG,BGGR,GBRG).");
                break;

            case MODE_BAYER_RGGB:
            case MODE_BAYER_GRBG:
            case MODE_BAYER_BGGR:
            case MODE_BAYER_GBRG:
                throw  std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode bgrb to bayer pattern. Conversion is not implemented.");
                break;

            default:
                throw std::runtime_error("FrameHelper::convertColor: Cannot convert frame mode - mode is unknown");
            }
            break;


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
                dst.frame_mode = src.frame_mode;
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
    void FrameHelper::convertRGBToGray(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst,bool copy_attributes)
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


    //assumption 
    //the size of the object does not change with its position on the image plane
    cv::Point2f FrameHelper::calcRelPosToPoint(float fx,float fy, int x1,int y1,int x2,int y2, float d)
    {
        cv::Point2f point;
        point.x = (x1-x2)*d/fx;
        point.y = (y1-y2)*d/fy;
        return point;
    }

    //assumption 
    //the size of the object does not change with its position on the image plane
    float FrameHelper::calcDistanceToObject(float f,float virtual_size,float real_size)
    {
        return real_size*f/virtual_size;
    }

    cv::Point2f FrameHelper::calcRelPosToPoint(const base::samples::frame::Frame &frame,
            int x1,int y1,int x2,int y2, float d)
    {
        if(!frame.hasAttribute("fx"))
            throw std::runtime_error("FrameHelper::calcDistanceToObject: frame has no attribute fx");
        if(!frame.hasAttribute("fy"))
            throw std::runtime_error("FrameHelper::calcDistanceToObject: frame has no attribute fy");

        float fx = frame.getAttribute<float>("fx"); 
        float fy = frame.getAttribute<float>("fy"); 
        return calcRelPosToPoint(fx,fy,x1,y1,x2,y2,d);
    }

    cv::Point2f FrameHelper::calcRelPosToCenter(const base::samples::frame::Frame &frame,
            int x1,int y1, float d)
    {
        return calcRelPosToPoint(frame,x1,y1,frame.getWidth()*0.5,frame.getHeight()*0.5,d);
    }

    float FrameHelper::calcDistanceToObject(const base::samples::frame::Frame &frame,
            float virtual_width,float real_width,
            float virtual_height,float real_height)
    {
        if(virtual_width > virtual_height)
        {
            if(!frame.hasAttribute("fx"))
                throw std::runtime_error("FrameHelper::calcDistanceToObject: frame has no attribute fx");
            float fx = frame.getAttribute<float>("fx"); 
            return calcDistanceToObject(fx,virtual_width,real_width);
        }

        if(!frame.hasAttribute("fy"))
            throw std::runtime_error("FrameHelper::calcDistanceToObject: frame has no attribute fy");

        float fy = frame.getAttribute<float>("fy"); 
        return calcDistanceToObject(fy,virtual_height,real_height);
    }

    void FrameHelper::convertBayerToRGB24(const uint8_t *src, uint8_t *dst, int width, int height, base::samples::frame::frame_mode_t mode)
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

    void FrameHelper::calcDiff(const base::samples::frame::Frame &src1,const base::samples::frame::Frame &src2,base::samples::frame::Frame &dst)
    {
        int frame_total_size = src1.getNumberOfBytes();

        if(frame_total_size != src2.getNumberOfBytes())
            throw std::runtime_error("calcDiff: size missmatch between src1 and src2 --> can not calc diff! ");
        if(src1.data_depth != 8)
            throw std::runtime_error("calcDiff: only 8 bit data depth is supported!");

        const uint8_t *p_src1 = src1.getImageConstPtr();
        const uint8_t *p_src2 = src2.getImageConstPtr();

        dst.init(src1,false);
        uint8_t *p_dst = dst.getImagePtr();

        for(int i=0;i < frame_total_size;++i)
        {
            if(*p_src1 > *p_src2)
                *p_dst = *p_src1-*p_src2;
            else
                *p_dst = 0;//*p_src2-*p_src1;
            ++p_dst;
            ++p_src1;
            ++p_src2;
        }
    }

    void FrameHelper::convertBayerToGreenChannel(const base::samples::frame::Frame &src,base::samples::frame::Frame &dst)
    {
        //check dst format 
        if(dst.frame_mode != MODE_GRAYSCALE)
            std::runtime_error("FrameHelper::converBayerToGreenChannel: Cannot convert frame mode bayer to green channel. Dst image must be of mode MODE_GRAYSCALE.");

        if(src.getDataDepth() != dst.getDataDepth())
            std::runtime_error("FrameHelper::converBayerToGreenChannel: Cannot convert frame mode bayer to green channel. src and dst must have the same data depth.");

        dst.init(src.getWidth(),src.getHeight(),src.getDataDepth(),dst.getFrameMode(),-1);
        convertBayerToGreenChannel(src.getImageConstPtr(),dst.getImagePtr(),src.getWidth(),src.getHeight(),src.frame_mode);	
        dst.copyImageIndependantAttributes(src);
    }

    //reads the green channel from an image which uses a bayer pattern
    void FrameHelper::convertBayerToGreenChannel(const uint8_t *src, uint8_t *dst, int width, int height, base::samples::frame::frame_mode_t mode)
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

    void FrameHelper::copyMatToFrame(const cv::Mat &src, base::samples::frame::Frame &frame)
    {
        frame_mode_t mode;
        int color_depth;
        switch (src.type())
        {
        case CV_8UC1:
            mode = MODE_GRAYSCALE;
            color_depth = 8;
            break;
        case CV_8UC3:
            mode = MODE_RGB;
            color_depth = 8;
            break;
        default:
            throw "Unknown format. Can not convert cv:Mat to Frame.";
        }
        frame.init(src.cols,src.rows,color_depth,mode);
        cv::Mat dst = frame.convertToCvMat();

        //this is only working if dst has the right size otherwise
        //cv is allocating new memory
        src.copyTo(dst);
    }
}
