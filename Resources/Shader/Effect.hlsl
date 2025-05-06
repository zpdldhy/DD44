#include "Header.hlsli"

VS_OUT_RIM VS(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.wPos = worldPos.xyz;
    output.n = normalize(mul(input.n, (float3x3) g_matWorld));
    output.c = input.c;
    output.t = input.t;

    return output;
}

float4 PS(VS_OUT_RIM input) : SV_Target
{
    if (g_iRenderMode == 1)
        return float4(0.2f, 0.2f, 0.2f, 1.0f); // 회색 실루엣

    // UV 왜곡
    float2 distortedUV = GetDistortedUV(input.t);

    // Dissolve 적용 여부 판단
    if (ShouldDissolve(distortedUV))
        clip(-1);

    // 텍스처 샘플링
    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
    float3 finalColor = texColor.rgb;

    // Glow 적용
    finalColor = ApplyGlow(finalColor);

    // Rim Light 적용
    finalColor += ApplyRimLight(input.n, input.wPos);

    // Hit Flash 적용
    finalColor = ApplyHitFlash(finalColor);

    return float4(finalColor, texColor.a);
}
