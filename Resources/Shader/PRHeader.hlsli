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

cbuffer CB_Debug : register(b3)
{
    int g_iDebugMode;
    float3 padding_Debug;
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

Texture2D g_txDefault : register(t0);   // 원본
Texture2D g_txBlur : register(t1);      // Blur
Texture2D g_txMask : register(t2);      // Mask
Texture2D g_txBloom : register(t3);     // Bloom
Texture2D g_txLight : register(t4);     // Light
Texture2D g_txNormal : register(t5);    // Normal
Texture2D g_txDepth : register(t6);     // Depth
Texture2D g_txTemp : register(t7);     // Temp

SamplerState sample : register(s0);


cbuffer CB_Blur : register(b11)
{
    float2 g_vTexelSize; // (1 / 화면 너비, 1 / 화면 높이)
    float2 g_vDirection; // (1, 0): 가로 블러 / (0, 1): 세로 블러
    float g_fBlurScale;
    float3 padding_blur;
};

#endif