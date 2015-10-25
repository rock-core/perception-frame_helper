#include "ColorGradient.h"
#include <algorithm>

namespace frame_helper {
// Inserts a new color point into its correct position
void ColorGradient::addColorPoint(const float red, const float green, const float blue, const float value) {
    for (std::list<ColorPoint>::iterator it = color.begin(); it != color.end(); ++it) {
        if (value < it->val) {
            color.insert(it, ColorPoint(red, green, blue, value));
            return;
        }
    }
    color.push_back(ColorPoint(red, green, blue, value));
}

// Inserts a new color point into its correct position
void ColorGradient::clearGradient() {
    color.clear();
}

// Places a jet color gradient into the color vector
void ColorGradient::createJetMapGradient() {
    color.push_back(ColorPoint(0, 0, 1, 0.0f));      // blue
    color.push_back(ColorPoint(0, 1, 1, 0.25f));     // cyan
    color.push_back(ColorPoint(0, 1, 0, 0.5f));      // green
    color.push_back(ColorPoint(1, 1, 0, 0.75f));     // yellow
    color.push_back(ColorPoint(1, 0, 0, 1.0f));      // red
}

// Places a hot color gradient into the color vector
void ColorGradient::createHotMapGradient() {
    color.push_back(ColorPoint(0, 0, 0, 0.0f));     // black
    color.push_back(ColorPoint(1, 0, 0, 0.125f));   // red
    color.push_back(ColorPoint(1, 1, 0, 0.55f));    // yellow
    color.push_back(ColorPoint(1, 1, 1, 1.0f));     // white
}

// Places a grayscale color gradient into the color vector
void ColorGradient::createGrayscaleMapGradient() {
    color.push_back(ColorPoint(0, 0, 0, 0.0f));     // black
    color.push_back(ColorPoint(1, 1, 1, 1.0f));     // white
}

// Inputs a (value) between 0 and 1 and outputs the (red), (green) and (blue)
// values representing that position in the gradient
void ColorGradient::getColorAtValue(const float value, float &red, float &green, float &blue) const {
    if (color.empty())
        throw std::out_of_range("ERROR: There is no color in the current palette.");

    int index = 0;
    for (std::list<ColorPoint>::const_iterator it = color.begin(); it != color.end(); ++it, ++index) {
        ColorPoint currC = *it;
        if (value < currC.val) {
            std::list<ColorPoint>::const_iterator temp = color.begin();
            std::advance(temp, std::max(0, index - 1));
            ColorPoint prevC = *temp;
            float valueDiff = (prevC.val - currC.val);
            float fractBetween = (valueDiff == 0) ? 0 : (value - currC.val) / valueDiff;
            red = (prevC.r - currC.r) * fractBetween + currC.r;
            green = (prevC.g - currC.g) * fractBetween + currC.g;
            blue = (prevC.b - currC.b) * fractBetween + currC.b;
            return;
        }
    }
    red = color.back().r;
    green = color.back().g;
    blue = color.back().b;
    return;
}

// Sets the current colormap
void ColorGradient::colormapSelector(const ColorGradientType type) {
    clearGradient();

    switch (type) {
        case COLORGRADIENT_JET:
            createJetMapGradient();
            break;

        case COLORGRADIENT_HOT:
            createHotMapGradient();
            break;

        case COLORGRADIENT_GRAYSCALE:
            createGrayscaleMapGradient();
            break;
        default:
            break;
    }
}

}
