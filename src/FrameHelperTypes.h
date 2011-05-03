#ifndef FRAMEHELPERTYPES_H
#define FRAMEHELPERTYPES_H

//structure for camera calibration 
namespace frame_helper
{
    struct CalibrationParameters
    {
        double fx, fy, cx, cy, d0, d1, d2, d3; 
        double tx, ty, tz;
        double rx, ry, rz;
        CalibrationParameters():
            fx(0),fy(0),cx(0),cy(0),d0(0),d1(0),d2(0),d3(0),tx(0),ty(0),tz(0),rx(1),ry(1),rz(1){};
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
};
#endif
