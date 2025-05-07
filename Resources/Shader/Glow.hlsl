#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    output.p = float4(input.p, 1.0f);
    output.c = input.c;
    output.n = input.n;
    output.t = input.t;
    return output;
}

PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;

    // 기본 텍스처
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);

    // Glow 색 계산
    float3 glow = g_vGlowColor.rgb * g_fGlowPower;

    // 텍스처 + Glow 조합
    float3 finalColor = texColor.rgb + glow;
    
    psOut.c = float4(saturate(finalColor), texColor.a);
    return psOut;
}