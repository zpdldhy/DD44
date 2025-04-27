#ifndef COMMON_HLSLI
#define COMMON_HLSLI
cbuffer cb0 : register(b0)
{
    matrix g_matWorld;
    matrix g_matView;
    matrix g_matProj;
}

cbuffer TransformBuffer : register(b1)
{
    matrix obj_matWorld;
};

cbuffer cbGlowFX : register(b2)
{
    float g_fGlowPower; // �߱� ����
    float3 g_vGlowColor; // �߱� ����
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
SamplerState sample : register(s0);
#endif