#include "Header.hlsli"

VS_OUT_RIM VS(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.wPos = worldPos.xyz;
    
    float3x3 worldMat3 = (float3x3) g_matWorld;
    output.n = normalize(mul(input.n, worldMat3));
    
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    //output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    output.c = input.c;
    output.t = input.t;

    float4 shadowCoord = mul(mul(worldPos, g_matShadowView), g_matShadowProj);
    shadowCoord.y = -shadowCoord.y;
    output.shadowCoord = shadowCoord;
    
    return output;
}

PS_OUT PS(VS_OUT_RIM input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;
    
    if (g_iRenderMode == 1)
    {
        float4 silhouette = float4(0.2f, 0.2f, 0.2f, 1.0f);
        output.c = silhouette;
        output.c1 = silhouette;
        output.c2 = silhouette;
        return output;
    }

    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    float3 baseColor = texColor.rgb;

    float3 shadowTexCoord = input.shadowCoord.xyz / input.shadowCoord.w;
    shadowTexCoord.xy = shadowTexCoord.xy * 0.5f + 0.5f;

    float shadow = 1.0f;
    if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
    {
        shadow = 0.0f;
    }
    else
    {
        shadow = g_txShadow.SampleCmpLevelZero(g_samShadow, shadowTexCoord.xy, shadowTexCoord.z - 0.001f);
        shadow += 0.5f;
    }
    
    
    float3 ambient = ApplyAmbient();
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 litColor = baseColor * (ambient + diffuse) + specular;
    litColor *= shadow;
    float3 emissive = g_vEmissiveColor * g_fEmissivePower;
    // 최종 출력용은 모든 효과를 포함
    float3 finalColor = litColor + emissive;
    finalColor = ApplyGlow(finalColor);
    finalColor = ApplyHitFlash(finalColor);
    
    
    // output.c : 최종 결과
    output.c = float4(finalColor, texColor.a);

    //shadowTexCoord = input.shadowCoord.xyz / input.shadowCoord.w;
    //output.c = float4(shadowTexCoord.xy, shadowTexCoord.z, 1);

    // output.c1 : Blur 대상용 (보통 최종 결과와 동일하게 해도 무방)
    output.c1 = float4(finalColor, texColor.a);

    // output.c2 : Bloom 대상용 (emissive 만 따로 추출)
    //output.c2 = float4(emissive, 1.0f); // 또는 threshold 기반으로 추출할 수도 있음
    output.c2 = float4(0.f, 0.f, 1.f, 1.f); //float4(finalColor, texColor.a); // 또는 threshold 기반으로 추출할 수도 있음
    
    output.c4.rgb = (ambient + diffuse) + specular;
    output.c4.a = 1.f;
    output.c5.rgb = input.n;
    output.c5.a = 1.f;
    output.c6 = float4(input.p.z / input.p.w, 0.0f, 0.0f, 1.0f); // R 채널에만 깊이
    
    return output;
}