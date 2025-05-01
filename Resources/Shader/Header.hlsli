#ifndef COMMON_HLSLI
#define COMMON_HLSLI
#define MAX_BONE 250

cbuffer cb0 : register(b0)
{
    row_major matrix g_matWorld;
}

cbuffer CameraBuffer : register(b1)
{
    row_major matrix g_matView;
    row_major matrix g_matProj;
};

cbuffer cbGlowFX : register(b2)
{
    float g_fGlowPower; // 발광 세기
    float3 padding;
    float3 g_vGlowColor; // 발광 색상
    float dummy;
    float g_fHitFlashTime;
}
cbuffer cbDissolve : register(b3)
{
    float g_fDissolveThreshold;
}
cbuffer CB_UVDistortion : register(b4)
{
    float g_fDistortionStrength; // 왜곡 세기 (예: 0.01 ~ 0.1)
    float g_fWaveSpeed; // 시간 흐름 속도 (예: 1.0)
    float g_fWaveFrequency; // 파장 (예: 10.0)
    float g_fDistortionTime; // 누적 시간 (프레임마다 누적됨)
};

cbuffer AnimationBuffer : register(b5)
{
    matrix obj_matAnim[MAX_BONE];
}

struct VS_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
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

struct VS_OUT
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
};

struct PS_OUT
{
    float4 c : SV_Target;
};

Texture2D g_txDiffuseA : register(t0);
Texture2D g_txNoise : register(t1); // 노이즈 텍스처
SamplerState sample : register(s0);
#endif