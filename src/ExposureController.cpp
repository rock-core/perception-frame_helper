#include <ExposureController.h>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;

int ExposureController::calcNewValue(int measuredValue, int target) {
	if(measuredValue == 0) {
		return lastExp + 1000;
	}
	if(abs(measuredValue - target) > tolerance) { 
		double mx = lastExp * 1.0 / measuredValue;
		cout << "MX:" << mx << endl;
		return mx * target;
	} else {
		return lastExp;
	}
}	

int ExposureController::update(int measuredValue, int target) {
	int value = calcNewValue(measuredValue, target);
	value = std::min(max, value);
	value = std::max(min, value);
	lastExp = value;

	return value;
}
