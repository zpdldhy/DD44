
#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    // NDC를 위한 위치 변환
    float4 local = float4(input.p.xy, 0.0f, 1.0f);
    output.p = mul(local, g_matWorld); // 스케일, 위치 반영
    output.t = input.t; // UV 그대로 사용

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    float4 result = g_txDiffuseA.Sample(sample, input.t);
    if (result.a < 0.4f)
    {
        discard;
    }
    
    return result;
}
