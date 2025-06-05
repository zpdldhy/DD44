#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    output.p = float4(input.p.xy, 0.0f, 1.0f); // NDC 바로 출력

    output.t = input.t;
    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    return g_txDiffuseA.Sample(sample, input.t);
}