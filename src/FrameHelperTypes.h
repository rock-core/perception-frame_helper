#ifndef FRAMEHELPERTYPES_H
#define FRAMEHELPERTYPES_H

//structure for camera calibration 
namespace frame_helper
{
    struct CalibrationParameterMono
    {
        double fx, fy, cx, cy, d0, d1, d2, d3; 
    };

    struct CalibrationParameterStereo
    {
        CalibrationParameterMono left_camera;
        CalibrationParameterMono rigth_camera;
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
};
#endif
