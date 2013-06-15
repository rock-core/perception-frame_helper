#include <ExposureController.h>
#include <algorithm>
#include <iostream>
#include <cmath>

LinearExposureController::LinearExposureController(int targetBrightness, int factor) {
	this->targetBrightness = targetBrightness;
	this->factor = factor;
}
void LinearExposureController::setExposureBounds(int minExposure, int maxExposure) {
	this->upperExposureBound = maxExposure;
	this->lowerExposureBound = minExposure;
}
void LinearExposureController::setCurrentExposure(int exposure) {
	this->currentExposure = exposure;
}
int LinearExposureController::getNewExposure(int currentBrightness) {
	int diff = targetBrightness-currentBrightness;
	int exposure_correction = diff * factor;
	//std::cout << "Befor pow:" << exposure_correction << std::endl;
	//exposure_correction = pow(exposure_correction * 1.0, 1.1);
	//std::cout << "After pow:" << exposure_correction << std::endl;
	currentExposure += exposure_correction;

	currentExposure = std::min(upperExposureBound, currentExposure);
	currentExposure = std::max(lowerExposureBound, currentExposure);

	return currentExposure; 
}
