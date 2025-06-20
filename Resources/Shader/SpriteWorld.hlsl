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

VS_OUT VS_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.p, 1.0f), input.matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.p = mul(viewPos, g_matProj);
    output.t = input.t;

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    float2 uv = input.t;
    float4 texColor = g_txDiffuseA.Sample(sample, uv);

    if (texColor.a < 0.1f)
        discard;

    // 중심으로부터 거리 계산 (0 = 중심, 1 = 바깥)
    float distFromCenter = distance(uv, float2(0.5f, 0.5f));
    float edgeFactor = saturate(distFromCenter * 2.0f);

    //// 중심은 흰색, 외곽은 연한 푸른색 보간
    //float3 centerColor = float3(1.0f, 1.0f, 1.0f);
    //float3 outerTint = float3(0.6f, 0.8f, 1.0f); // 연한 파랑

    //float3 finalColor = lerp(centerColor, outerTint, edgeFactor);
    //texColor.rgb *= finalColor;

    // 알파 점점 줄이기
    float alphaFalloff = 1.0f - edgeFactor * 0.5f;
    texColor.a *= alphaFalloff;

    return texColor;
}