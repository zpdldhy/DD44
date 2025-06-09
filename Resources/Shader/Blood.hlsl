#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld); // ← 수정
    output.p = mul(mul(worldPos, g_matView), g_matProj); // 그대로 유지

    output.t = input.t;
    return output;
}

PS_OUT_DUAL PS(VS_OUT input)
{
    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    if (texColor.a < 0.1f)
        discard;

    float alpha = max(max(texColor.r, texColor.g), texColor.b);

    psOut.c0 = float4(texColor.rgb, texColor.a);
    psOut.c1 = float4(1.0f - texColor.rgb, alpha); // Dual Blend용 보조색

    return psOut;
}



