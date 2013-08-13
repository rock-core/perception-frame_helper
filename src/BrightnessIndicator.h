#ifndef BRIGHTNESSINDICATOR_H
#define BRIGHTNESSINDICATOR_H

#include <cv.h>
#include <vector>
#include "base/samples/frame.h"


class WeightedRect {
	public:
		float x1, y1;
		float x2, y2;
		int weight;

		WeightedRect(float _x1, float _y1, float _x2, float _y2, int _weight);

	private:
		bool inRange(int number);
};

class AbstractBrightnessIndicator {
	public:
		/* Returns a number indicating how bright the image is in the range from 0 - 255 */
		virtual int getBrightness(cv::Mat frame) = 0;
};

/*
 * Uses the mean value of all pixels as brightness indicator for the image
 */
class SimpleBrightnessIndicator : AbstractBrightnessIndicator {
	public: 
		virtual int getBrightness(cv::Mat frame);
};

/*
 * Uses the weighted sum of the brightness in different regions in the picture
 * as brightness indicator. The brightness in the picture is calculated by the
 * mean value of all pixels in it.
 */
class WeightedBoxesBrightnessIndicator : AbstractBrightnessIndicator {
	private:
		std::vector<WeightedRect> weightedRegions;
	public:
		WeightedBoxesBrightnessIndicator(std::vector<WeightedRect> _weightedRegions) : weightedRegions(_weightedRegions) {}; 
		void setWeightedRegions(std::vector<WeightedRect> _weightedRegions);
		virtual int getBrightness(cv::Mat frame);

		const static std::vector<WeightedRect> REGION_SPOT;
		const static std::vector<WeightedRect> REGION_BOTTOM_DOUBLE;
};	

#endif /* end of include guard: BRIGHTNESSINDICATOR_H */
