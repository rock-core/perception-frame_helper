#ifndef FRAMEHELPERTYPES_H
#define FRAMEHELPERTYPES_H

//structure for camera calibration 
struct CalibrationParameters
{
        double fx, fy, cx, cy, d0, d1, d2, d3;
        double tx, ty, tz;
        double rx, ry, rz;
};

enum ResizeAlgorithm
{
    INTER_LINEAR,
    INTER_NEAREST,
    INTER_AREA,
    INTER_CUBIC,
    INTER_LANCZOS4,
    BAYER_RESIZE
};
#endif
