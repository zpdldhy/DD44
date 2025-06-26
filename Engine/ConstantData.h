#pragma once
#include "pch.h"

struct alignas(16) LightData
{
    Vec4 vLightColor_Intensity = Vec4();       // (RGB, Intensity)
    Vec4 vLightDirection_Range = Vec4();       // (XYZ, Range)
    Vec4 vLightPosition_Angle = Vec4();        // (XYZ, Angle)
    Vec4 vType_AmbientColor = Vec4();          // (iLightType, AmbientColor.xyz)
    Vec4 vAmbientPower_Padding = Vec4();       // (AmbientPower, padding[3])
};

struct alignas(16) CB_LightArray
{
    LightData lights[4] = { LightData(), };
    int iNumLights = 0;
    Vec3 padding = Vec3(); // align to 16
};

struct CameraConstantData
{
    Matrix matView = Matrix();
    Matrix matProjection = Matrix();
    Vec3   g_vCameraPos = Vec3();
    float  GameTime = 0.f;
    
};