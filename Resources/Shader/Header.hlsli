#include "MainHeader.hlsli"
#define MAX_BONE 250
#define MAX_LIGHTS 4

//cbuffer cbDissolve : register(b3)
//{
//    float g_fDissolveThreshold;
//}

cbuffer CB_SpriteUV : register(b4)
{
    float2 g_uvStart;
    float2 g_uvEnd;
}

cbuffer AnimationBuffer : register(b5)
{
    matrix obj_matAnim[MAX_BONE];
}

cbuffer CB_Billboard : register(b6)
{
    float3 g_vBillboardCenter;
    float  g_fBillboardRotation;
    float2 g_vBillboardSize;
    float2 padding_size;
};

cbuffer CB_RenderMode : register(b7)
{
    int g_iRenderMode;
    float3 padding_rendermode;
};

struct LightData
{
    float4 vLightColor_Intensity; // (RGB, Intensity)
    float4 vLightDirection_Range; // (XYZ, Range)
    float4 vLightPosition_Angle; // (XYZ, Angle)
    float4 vType_AmbientColor; // (iLightType, Ambient RGB)
    float4 vAmbientPower_Padding; // (AmbientPower, padding)
};

cbuffer CB_LightArray : register(b8)
{
    LightData g_arrLights[MAX_LIGHTS];
    int g_iNumLights;
    float3 padding_light;
}

cbuffer InverseBoneBuffer : register(b9)
{
    matrix obj_matBone[MAX_BONE];
}

cbuffer CB_Slash : register(b10)
{
    float g_fProgress; 
    float3 padding_slash;
}

cbuffer CB_Blur : register(b11)
{
    float2 g_vTexelSize; // (1 / 화면 너비, 1 / 화면 높이)
    float2 g_vDirection; // (1, 0): 가로 블러 / (0, 1): 세로 블러
};

cbuffer CB_Wind : register(b12) 
{
    float2 g_vUVStart;
    float2 g_vUVSize;
    float2 g_vUVOffset;
    float2 padding;
};


struct PNCTIW_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;

    float4 i : INDEX;
    float4 i2 : SECONDI;
    float4 w : WEIGHT;
    float4 w2 : SECONDW;
};

struct VS_OUT_RIM
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
    float3 wPos : POSITIONWS;
};

///funtion
//==============================
// [FX] Shared Utility Functions
//==============================

float3 UnpackLightColor(int index)
{
    return g_arrLights[index].vLightColor_Intensity.rgb;
}
float UnpackIntensity(int index)
{
    return g_arrLights[index].vLightColor_Intensity.a;
}
float3 UnpackLightDirection(int index)
{
    return g_arrLights[index].vLightDirection_Range.rgb;
}
float UnpackRange(int index)
{
    return g_arrLights[index].vLightDirection_Range.a;
}
float3 UnpackLightPosition(int index)
{
    return g_arrLights[index].vLightPosition_Angle.rgb;
}
float UnpackAngle(int index)
{
    return g_arrLights[index].vLightPosition_Angle.a;
}
int UnpackType(int index)
{
    return (int) g_arrLights[index].vType_AmbientColor.x;
}
float3 UnpackAmbientColor(int index)
{
    return g_arrLights[index].vType_AmbientColor.yzw;
}
float UnpackAmbientPower(int index)
{
    return g_arrLights[index].vAmbientPower_Padding.x;
}

float3 ApplyLambertLighting(float3 normal, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < g_iNumLights; ++i)
    {
        float3 lightDir;
        float attenuation = 1.0f;

        int type = UnpackType(i);
        if (type == 0)
        {
            lightDir = normalize(-UnpackLightDirection(i));
        }
        else
        {
            float3 toLight = UnpackLightPosition(i) - worldPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);
            attenuation = saturate(1.0 - dist / UnpackRange(i));

            if (type == 2)
            {
                float3 spotDir = normalize(-UnpackLightDirection(i));
                float spotFactor = dot(lightDir, spotDir);
                float spotCos = cos(radians(UnpackAngle(i)));
                float spotEffect = smoothstep(spotCos, spotCos + 0.1f, spotFactor);
                attenuation *= spotEffect;
            }
        }

        float NdotL = saturate(dot(normal, lightDir));
        result += UnpackLightColor(i) * NdotL * UnpackIntensity(i) * attenuation * g_vDiffuseCoeff * g_fDiffusePower;
    }
    return result;
}



float3 ApplySpecular(float3 normal, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < g_iNumLights; ++i)
    {
        float3 lightDir;
        float attenuation = 1.0f;

        int type = UnpackType(i);
        if (type == 0)
        {
            lightDir = normalize(-UnpackLightDirection(i));
        }
        else
        {
            float3 toLight = UnpackLightPosition(i) - worldPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);
            attenuation = saturate(1.0 - dist / UnpackRange(i));

            if (type == 2)
            {
                float3 spotDir = normalize(-UnpackLightDirection(i));
                float spotFactor = dot(lightDir, spotDir);
                float spotCos = cos(radians(UnpackAngle(i)));
                float spotEffect = smoothstep(spotCos, spotCos + 0.1f, spotFactor);
                attenuation *= spotEffect;
            }
        }

        float3 N = normalize(normal);
        float3 V = normalize(g_vCameraPos - worldPos); // ← 여기 반대 실험도 가능
        float3 R = reflect(-lightDir, N);

        float NdotL = dot(N, lightDir);
        if (NdotL <= 0.0f)
            continue; // ← Backface 억제

        float spec = pow(saturate(dot(R, V)), g_fShininess);
        result += UnpackLightColor(i) * spec * UnpackIntensity(i) * attenuation * g_vSpecularCoeff;
    }
    return result;
}

float3 ApplyAmbient()
{
    return UnpackAmbientColor(0) * UnpackAmbientPower(0) * g_vAmbientCoeff * g_fAmbientPower;
}


float2 remapUV(float2 uv)
{
    return lerp(g_uvStart, g_uvEnd, uv);
}
