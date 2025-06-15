#pragma once
#include "pch.h"

struct alignas(16) LightData
{
    Vec4 vLightColor_Intensity;       // (RGB, Intensity)
    Vec4 vLightDirection_Range;       // (XYZ, Range)
    Vec4 vLightPosition_Angle;        // (XYZ, Angle)
    Vec4 vType_AmbientColor;          // (iLightType, AmbientColor.xyz)
    Vec4 vAmbientPower_Padding;       // (AmbientPower, padding[3])
};

struct alignas(16) CB_LightArray
{
    LightData lights[4];
    int iNumLights;
    Vec3 padding; // align to 16
};

struct CameraConstantData
{
    Matrix matView;
    Matrix matProjection;
    Vec3   g_vCameraPos;
    float  GameTime;
    Matrix matShadowView;
    Matrix matShadowProj;

};