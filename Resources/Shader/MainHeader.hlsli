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
    float3 g_vCameraPos;
    float dummy_camera;
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
    float4 c : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
    float4 c3 : SV_Target3;
    float4 c4 : SV_Target4;
    float4 c5 : SV_Target5;
    float4 c6 : SV_Target6;
    float4 c7 : SV_Target7;
};

Texture2D g_txDiffuseA : register(t0);
//Texture2D g_txNoise : register(t1); // 노이즈 텍스처
SamplerState sample : register(s0);

#endif