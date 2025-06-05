#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 local = float4(input.p.xy, 0.0f, 1.0f); // 기본 Plane vertex (-0.5 ~ +0.5)
    float4 world = mul(local, g_matWorld); // 월드 변환 (스케일+위치 포함)

    output.p = float4(world.xy, 0.0f, 1.0f); //  그대로 NDC 출력
    output.t = input.t;
    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    return g_txDiffuseA.Sample(sample, input.t);
}