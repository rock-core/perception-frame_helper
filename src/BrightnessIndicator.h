#ifndef BRIGHTNESSINDICATOR_H
#define BRIGHTNESSINDICATOR_H

#include <cv.h>
#include <vector>
#include "base/samples/frame.h"

class AbstractBrightnessIndicator {
	public:
		/* Returns a number indicating how bright the image is in the range from 0 - 255 */
		virtual int getBrightness(cv::Mat frame) = 0;
};

class SimpleBrightnessIndicator : AbstractBrightnessIndicator {
	public: 
		virtual int getBrightness(cv::Mat frame);
};

class WeightedBoxesBrightnessIndicator : AbstractBrightnessIndicator {
	public:
		virtual int getBrightness(cv::Mat frame);
};	

#endif /* end of include guard: BRIGHTNESSINDICATOR_H */
