#ifndef EXPOSURECONTROLLER_H

#define EXPOSURECONTROLLER_H

/*
 * Controller for the exposure time of video cameras
 * The Controller assumes that the camera detector hat a linear relationship between exposure time and brightness of the picture.
 */
class ExposureController {
	private:
		int tolerance, min, max;
		int lastExp;
		int calcNewValue(int measuredValue, int target);
	public:
		/*
		 * Constructor
		 * @param _min Minimum Exposure time
		 * @param _max Maximum Exposure time
		 * @param _tolerance No new exposure time will be calculated if distance to target value is below this value
		 * @param _currentExp The current exposure time of the camera
		 */
		ExposureController(int _min, int _max, int _tolerance, int _currentExp) : tolerance(_tolerance), min(_min), max(_max), lastExp(_currentExp) {}

		/*
		 * Call this method to calculate a new exposure time for your camera.
		 * @param measuredValue The measured brightness of the current frame
		 * @param target The target brightness value for the next frames
		 * @return The new exposure time for the next frames
		 */
		int update(int measuredValue, int target);
};

#endif /* end of include guard: EXPOSURECONTROLLER_H */
