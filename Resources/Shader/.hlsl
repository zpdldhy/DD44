#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.p = mul(viewPos, g_matProj);
    output.t = input.t;

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
     if (input.t.x > g_fProgress)
       discard;

    float4 color = g_txDiffuseA.Sample(sample, input.t);
    return color;
}