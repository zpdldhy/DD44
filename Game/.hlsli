#include "Header.hlsli"

cbuffer cb0 : register(b0)
{
    matrix g_matWorld;
    matrix g_matView;
    matrix g_matProj;
};

cbuffer cbSlashFX : register(b4)
{
    float g_fCutoff;
    float g_fGlow;
    float4 g_vColor;
    float4 g_vOccludedColor;
};

Texture2D g_txMain : register(t0); // Distortion map
SamplerState g_sampler : register(s0);

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output;
    float4 wPos = mul(float4(input.pos, 1.0f), g_matWorld);
    float4 vPos = mul(wPos, g_matView);
    output.pos = mul(vPos, g_matProj);
    output.uv = input.uv;
    return output;
}

float4 PS_SlashFX(VS_OUT input) : SV_Target
{
    float4 tex = g_txMain.Sample(g_sampler, input.uv);

    float glowMask = step(g_fCutoff, tex.r);
    float3 glow = lerp(g_vOccludedColor.rgb, g_vColor.rgb, glowMask);
    glow *= g_fGlow;

    return float4(glow, tex.a);
}
