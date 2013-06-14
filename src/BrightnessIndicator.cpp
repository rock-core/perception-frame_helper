#include "BrightnessIndicator.h"

using namespace cv;
using namespace std;

int SimpleBrightnessIndicator::getBrightness(Mat image) {
	cv::Scalar s = cv::mean(image);
	int sum = 0;
	for(int channel=0;channel < image.channels();++channel) {
		sum += s[channel];
	}
	return sum/image.channels();
}

int WeightedBoxesBrightnessIndicator::getBrightness(Mat image) {
	return 0;
}
