#include "Header.hlsli"

Texture2D g_txInput : register(t0);
SamplerState g_samp : register(s0);

struct BLUR_VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct BLUR_PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

BLUR_PS_IN VS(BLUR_VS_IN input)
{
    BLUR_PS_IN output;
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    return output;
}

float4 PS(BLUR_PS_IN input) : SV_Target
{
    float2 uv = input.uv;
    float4 color = float4(0, 0, 0, 0);

    float weights[5] =
    {
        0.227027f, // center
        0.1945946f,
        0.1216216f,
        0.0540541f,
        0.0162162f
    };

    color += g_txInput.Sample(g_samp, uv) * weights[0]; // center

    for (int i = 1; i < 5; ++i)
    {
        float2 offset = g_vDirection * g_vTexelSize * i * 10.0f;
        color += g_txInput.Sample(g_samp, uv + offset) * weights[i];
        color += g_txInput.Sample(g_samp, uv - offset) * weights[i];
    }

   //return color;
    return float4(1.f, 0.f, 0.f, 1.0f);
   // return float4(color.rgb, 1.0f);
    
}
