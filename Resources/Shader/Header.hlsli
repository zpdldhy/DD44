#ifndef COMMON_HLSLI
#define COMMON_HLSLI
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

struct VS_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
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