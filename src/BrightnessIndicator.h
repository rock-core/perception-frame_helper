#ifndef BRIGHTNESSINDICATOR_H
#define BRIGHTNESSINDICATOR_H

#include <cv.h>
#include <vector>
#include "base/samples/frame.h"


struct WeightedRect {
	int x, y;
	int width, height;
	int weight;

	WeightedRect(int _x, int _y, int _width, int _height, int _weight) : x(_x), y(_y), width(_width), height(_height), weight(_weight) {}
};

class AbstractBrightnessIndicator {
	public:
		/* Returns a number indicating how bright the image is in the range from 0 - 255 */
		virtual int getBrightness(cv::Mat frame) = 0;
};

class SimpleBrightnessIndicator : AbstractBrightnessIndicator {
	public: 
		virtual int getBrightness(cv::Mat frame);
};

class PercentageOverExposedIndicator : AbstractBrightnessIndicator {
	public: 
		virtual int getBrightness(cv::Mat frame);
};

class WeightedBoxesBrightnessIndicator : AbstractBrightnessIndicator {
	private:
		std::vector<WeightedRect> weightedRegions;
	public:
		WeightedBoxesBrightnessIndicator(std::vector<WeightedRect> _weightedRegions) : weightedRegions(_weightedRegions) {}; 
		void setWeightedRegions(std::vector<WeightedRect> _weightedRegions);
		virtual int getBrightness(cv::Mat frame);
};	

#endif /* end of include guard: BRIGHTNESSINDICATOR_H */
