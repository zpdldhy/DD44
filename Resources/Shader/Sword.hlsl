#define USE_BLOOM
#include "Header.hlsli"

VS_OUT_RIM VS(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.wPos = worldPos.xyz;
    //output.n = normalize(mul(input.n, (float3x3) g_matWorld));
    output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    output.c = input.c;
    output.t = input.t;

    return output;
}

PS_OUT PS(VS_OUT_RIM input)
{
    PS_OUT output;

    float4 texColor = g_txDiffuseA.Sample(sample, input.t);

    // Alpha 기준으로 금속 영역 판단
    bool isMetal = texColor.a < 0.2f;

    float3 finalColor = float3(0, 0, 0);
    float3 bloomColor = float3(0, 0, 0); // bloom에 보낼 별도 색

    if (isMetal)
    {
        float3 ambient = ApplyAmbient();
        float3 specular = ApplySpecular(input.n, input.wPos);
        finalColor = ambient + specular;

        // 메탈릭은 bloom 제외
    }
    else
    {
        float3 lit = ApplyLambertLighting(input.n);
        float3 emissive = g_vEmissiveColor.rgb * g_fEmissivePower;
        finalColor = texColor.rgb * lit + emissive;

        // emissive 기반 bloom 추출 (조건 설정 가능)
        bloomColor = emissive * step(0.2f, g_fEmissivePower); // 0.2 이상이면 bloom
    }

    finalColor = ApplyHitFlash(finalColor);

#ifdef USE_BLOOM
    output.color = float4(saturate(finalColor), 1.0f);
    output.bloom = float4(saturate(bloomColor), 1.0f); // 별도 추출
#else
    output.c = float4(saturate(finalColor), 1.0f);
#endif

    return output;
}