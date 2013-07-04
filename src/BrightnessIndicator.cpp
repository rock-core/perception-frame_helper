#include "BrightnessIndicator.h"

using namespace cv;
using namespace std;


static int calcBrightOfMat(Mat image) {
	cv::Scalar s = cv::mean(image);
	int sum = 0;
	for(int channel=0;channel < image.channels();++channel) {
		sum += s[channel];
	}
	return sum/image.channels();
}

int SimpleBrightnessIndicator::getBrightness(Mat image) {
	return calcBrightOfMat(image);
}

int WeightedBoxesBrightnessIndicator::getBrightness(Mat image) {
	int weightedSum = 0;
	int sumOfWeights = 0;
	for(std::vector<WeightedRect>::iterator i = weightedRegions.begin(); i != weightedRegions.end(); ++i) {
		int sum = calcBrightOfMat(Mat(image, cv::Rect(i->x, i->y, i->width, i->height))) * i->weight;
		weightedSum += sum; 
		//printf("Sum: %d weightedSum: %d\n", sum, weightedSum);
		sumOfWeights += i->weight;

	}
	return weightedSum / sumOfWeights;
}
void WeightedBoxesBrightnessIndicator::setWeightedRegions(std::vector<WeightedRect> _weightedRegions)
{
	weightedRegions = _weightedRegions;
}
int PercentageOverExposedIndicator::getBrightness(Mat image) {
	int overExposedPixels = 0;
	for(int x=0; x < image.cols; ++x) {
		for(int y=0; y < image.rows; ++y) {
			for(int channel = 0; channel < image.channels(); ++channel) {
				int value = image.data[
					 y * image.cols * image.channels() +
					 x * image.channels() +
					 channel];
				if(value > 250) {
					overExposedPixels += 1;
				}
			}
		}
	}
	return overExposedPixels * 100 / (image.cols * image.rows * image.channels());
}
