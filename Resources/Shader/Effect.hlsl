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

PS_OUT PS(VS_OUT_RIM input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;
    
    // 실루엣 렌더링 모드
    if (g_iRenderMode == 1)
    {
        output.c = float4(0.2f, 0.2f, 0.2f, 1.0f);
        output.c1 = float4(0.2f, 0.2f, 0.2f, 1.0f);
        output.c2 = float4(0.2f, 0.2f, 0.2f, 1.0f);
        return output;
    }

    // 1. UV 왜곡
    float2 distortedUV = GetDistortedUV(input.t);

    // 2. Dissolve 클리핑
    if (ShouldDissolve(distortedUV))
        clip(-1);

    // 3. 텍스처 색상
    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
    float3 baseColor = texColor.rgb;

    // 4. 조명 계산
    float3 ambient = ApplyAmbient();
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 emissive = g_vEmissiveColor * g_fEmissivePower;

    // 5. 조명 적용
    float3 litColor = baseColor * (ambient + diffuse) + specular;

    // 6. Emissive 직접 더함
    litColor += emissive;

    // 7. Glow, HitFlash 등 후처리 효과
    litColor = ApplyGlow(litColor);
    litColor = ApplyHitFlash(litColor);

    // 8. 출력
    output.c = float4(litColor, texColor.a);
    output.c1 = float4(litColor, texColor.a);
    output.c2 = float4(0.f, 0.f, 0.f, 1.f);
    
    return output;
}