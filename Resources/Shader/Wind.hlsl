#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 local = float4(input.p.xy, 0.0f, 1.0f);
    output.p = mul(local, g_matWorld); // 위치, 스케일 반영

    output.t = input.t * g_vUVSize + g_vUVStart + g_vUVOffset;
    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    float4 result = g_txDiffuseA.Sample(sample, input.t);
    return float4(1, 0, 1, 0);
    //return result;

}