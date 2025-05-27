#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.p = mul(mul(worldPos, g_matView), g_matProj);

    output.c = input.c;
    output.n = input.n;
    output.t = input.t;

    return output;
}

PS_OUT_DUAL PS(VS_OUT input)
{
    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    
    if (texColor.a < 0.1)
        discard;

    float alpha = max(max(texColor.r, texColor.g), texColor.b);

    psOut.c0 = float4(texColor.rgb, 1.0f); // 원본 색상 출력
    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha); // 보조 블렌딩 채널

    return psOut;
}