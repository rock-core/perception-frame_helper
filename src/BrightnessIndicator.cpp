#include "BrightnessIndicator.h"

using namespace cv;
using namespace std;

static vector<WeightedRect> createRegionSpot() {
	vector<WeightedRect> spot;
	spot.push_back(WeightedRect(-0.3, -0.3, 0.3, 0.3, 1));
	return spot;
}

const vector<WeightedRect> WeightedBoxesBrightnessIndicator::REGION_SPOT(createRegionSpot());

static int calcBrightOfMat(Mat image) {
	cv::Scalar s = cv::mean(image);
	int sum = 0;
	for(int channel=0;channel < image.channels();++channel) {
		sum += s[channel];
	}
	return sum/image.channels();
}


bool WeightedRect::inRange(int number) {
	return -1.0 <= number && number <= 1.0;
}

WeightedRect::WeightedRect(float _x1, float _y1, float _x2, float _y2, int _weight) : weight(_weight) {
	if(!inRange(_x1) || !inRange(_y1) || !inRange(_x2) || !inRange(_y2)) {
		throw runtime_error("One of the parameters _x1, _y1, _x2, _y2 is out of bounds (-1.0 - 1.0)");
	}
	else {
		x1 = _x1;
		x2 = _x2;
		y1 = _y1;
		y2 = _y2;
	}
}

int SimpleBrightnessIndicator::getBrightness(Mat image) {
	return calcBrightOfMat(image);
}

int WeightedBoxesBrightnessIndicator::getBrightness(Mat image) {
	int weightedSum = 0;
	int sumOfWeights = 0;
	double x_center = image.cols * 1.0 / 2;
	double y_center = image.rows * 1.0 / 2;
	for(std::vector<WeightedRect>::iterator i = weightedRegions.begin(); i != weightedRegions.end(); ++i) {
		double x = x_center + (x_center * i->x1);
		double y = y_center + (y_center * i->y1);
		double width = (x_center + (x_center * i->x2)) - x;
		double height = (y_center + (y_center * i->y2)) - y;

		//Let the compiler round correctly for us...
		cv::Rect roi(x+0.5,y+0.5,width+0.5, height+0.5);
		/*printf("Image: Width: %d Height: %d\n", image.cols, image.rows);
		printf("ROI: X1: %f Y1: %f X2: %f Y2: %f\n", i->x1, i->y1, i->x2, i->y2);
		printf("ROI: X: %f Y: %f Width: %f Height: %f\n", x, y, width, height);
		printf("ROI: X: %d Y: %d Width: %d Height: %d\n", roi.x, roi.y, roi.width, roi.height);*/
		int sum = calcBrightOfMat(Mat(image, roi));
		weightedSum += sum * i->weight; 
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
