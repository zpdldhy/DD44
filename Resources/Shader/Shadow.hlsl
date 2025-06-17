#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matShadowWorld);
    worldPos = mul(worldPos, g_matShadowView);
    output.p = mul(worldPos, g_matShadowProj);
    
    return output;
}


float PS(VS_OUT input) : SV_Depth
{
    return input.p.z / input.p.w;
}