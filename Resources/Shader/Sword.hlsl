#include "Header.hlsli"

VS_OUT_RIM VS(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.wPos = worldPos.xyz;
    output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    output.c = input.c;
    output.t = input.t;

    return output;
}

VS_OUT VS_SHADOW(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    worldPos = mul(worldPos, g_matShadowView);
    output.p = mul(worldPos, g_matShadowProj);
    
    return output;
}

VS_OUT VS_SHADOW_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.p, 1.0f), input.matWorld);
    worldPos = mul(worldPos, g_matShadowView);
    output.p = mul(worldPos, g_matShadowProj);
    
    return output;
}

VS_OUT_RIM VS_BLOOM(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.wPos = worldPos.xyz;
    output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    output.t = input.t;

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
    float3 emissive;
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 ambient = ApplyAmbient();
    float3 flash;

    if (texColor.r < 0.6f)
    {
        emissive = float3(0, 0, 0);
        ambient = float3(1, 1, 1);
        specular = float3(0, 0, 0);
        output.c2 = float4(0.f, 0.f, 1.f, 1.f);
    }
    else
    {
        emissive = g_vEmissiveColor * g_fEmissivePower * 10.0f;
        output.c2 = float4(1.f, 1.f, 1.f, 1.f);
    }
    
    float3 baseColor = texColor.rgb;
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    

    float3 litColor = baseColor * (ambient + diffuse) + specular;

    
    float4 finalColor = float4(litColor + emissive, texColor.a);
    finalColor.rgb = ApplyGlow(finalColor.rgb);

    // output.c : 최종 결과
    output.c = float4(finalColor);

    // output.c1 : Blur 대상용 (보통 최종 결과와 동일하게 해도 무방)
    output.c1 = float4(finalColor);

 
    if (texColor.r > 0.3f)
    {
        float hitAmount = saturate(g_fHitFlashTime);
        float3 pureWhite = float3(1, 1, 1);
        float alpha = lerp(0.0f, 0.5f, hitAmount); // 약한 흰색 발광

        output.c3 = float4(pureWhite, alpha); // c3: HitFlash 전용 RTV
    }
    else
    {
        output.c3 = float4(0.114, 0.129, 0.125, 0); // 사용하지 않음
    }
    
    output.c4.rgb = (ambient + diffuse) + specular;
    output.c4.a = 1.f;
    output.c5.rgb = input.n;
    output.c5.a = 1.f;
    output.c6 = float4(input.p.z / input.p.w, 0.0f, 0.0f, 1.0f); // R 채널에만 깊이
    

    return output;
}

float PS_SHADOW(VS_OUT input) : SV_Depth
{
    return input.p.z / input.p.w;
}


float4 PS_BLOOM(VS_OUT_RIM input) : SV_Target
{
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    float3 emissive = float3(0, 0, 0);
    // 기준 컬러 필터링 (0.6 이상일 때만 발광)
    if (texColor.r < 0.6f)
    {
        return float4(0, 0, 0, 1);
    }
    else
    {
        emissive = float3(0.984, 0.069, 0.073) * 1.f;
    }
        
    return float4(emissive, 1.0f);
}