#ifndef EXPOSURECONTROLLER_H

#define EXPOSURECONTROLLER_H

class LinearExposureController {
	private:
		int targetBrightness;
		int factor;
		int upperExposureBound;
		int lowerExposureBound;
		int currentExposure;
		int tolerance;
	public:
		LinearExposureController(int targetBrightness, int factor, int tolerance);
		void setExposureBounds(int minExposure, int maxExposure);
		void setCurrentExposure(int exposure);
		int getNewExposure(int currentBrightness);
};

#endif /* end of include guard: EXPOSURECONTROLLER_H */
