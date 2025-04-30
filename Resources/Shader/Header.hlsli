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
    float g_fGlowPower; // ¹ß±¤ ¼¼±â
    float3 g_vGlowColor; // ¹ß±¤ »ö»ó
}

cbuffer AnimationBuffer : register(b3)
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
SamplerState sample : register(s0);
#endif