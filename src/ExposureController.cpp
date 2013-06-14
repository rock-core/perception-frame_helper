#include <ExposureController.h>

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
	currentExposure += exposure_correction;
	return currentExposure; 
}
