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

//float4 PS(VS_OUT_RIM input) : SV_Target
//{
//    // 실루엣 렌더링 모드
//    if (g_iRenderMode == 1)
//        return float4(0.2f, 0.2f, 0.2f, 1.0f);

//    // 1. UV 왜곡
//    float2 distortedUV = GetDistortedUV(input.t);

//    // 2. Dissolve 클리핑
//    if (ShouldDissolve(distortedUV))
//        clip(-1);

//    // 3. 텍스처 색상
//    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
//    float3 baseColor = texColor.rgb;

//    // 4. 조명 계산
//    float3 ambient = ApplyAmbient();
//    float3 diffuse = ApplyLambertLighting(input.n);
//    float3 specular = ApplySpecular(input.n, input.wPos);
//    float3 emissive = g_vEmissiveColor * g_fEmissivePower;

//    // 5. 조명 적용
//    float3 litColor = baseColor * (ambient + diffuse) + specular;

//    // 6. Emissive 직접 더함
//    litColor += emissive;

//    // 7. Glow, HitFlash 등 후처리 효과
//    litColor = ApplyGlow(litColor);
//    litColor = ApplyHitFlash(litColor);

//    // 8. 출력
//    return float4(litColor, texColor.a);
//}

PS_OUT PS(VS_OUT_RIM input)
{
    PS_OUT output;

    // 실루엣 렌더링 모드
    if (g_iRenderMode == 1)
    {
        output.color = float4(0.2f, 0.2f, 0.2f, 1.0f);
#ifdef USE_BLOOM
        output.bloom = float4(0, 0, 0, 1); // bloom 비움
#endif
        return output;
    }

    // 1. UV Distortion
    float2 distortedUV = GetDistortedUV(input.t);

    // 2. Dissolve
    if (ShouldDissolve(distortedUV))
        clip(-1);

    // 3. Texture Sample
    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
    float3 baseColor = texColor.rgb;

    // 4. Lighting
    float3 ambient = ApplyAmbient();
    float3 diffuse = ApplyLambertLighting(input.n);
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 emissive = g_vEmissive.yzw * g_vEmissive.x;

    float3 litColor = baseColor * (ambient + diffuse) + specular;
    litColor += emissive;

    // 5. Effects
    litColor = ApplyGlow(litColor);
    litColor = ApplyHitFlash(litColor);

    // 6. Output
#ifdef USE_BLOOM
    output.color = float4(saturate(litColor), texColor.a);
    output.bloom = float4(saturate(emissive), 1.0f); // bloom은 Emissive 기반
#else
    output.c = float4(saturate(litColor), texColor.a);
#endif

    return output;
}