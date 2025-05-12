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

Texture2D g_txDefault : register(t0);   // ¿øº»
Texture2D g_txBlur : register(t1);      // Blur
Texture2D g_txBloom : register(t2);     // Bloom

SamplerState sample : register(s0);
#endif