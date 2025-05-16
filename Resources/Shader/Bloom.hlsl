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
    float3 ambient =ApplyAmbient();
    float3 flash;

    if (texColor.r < 0.6f)
    {
        emissive = float3(0, 0, 0);
        ambient = float3(1, 1, 1);
        specular = float3(0, 0, 0);
        output.c2 = float4(1.f, 0.f, 1.f, 1.f); 
    }
    else
    {
        emissive = g_vEmissiveColor * g_fEmissivePower * 5.0f;
        output.c2 = float4(1.f, 1.f, 1.f, 1.f); 
    }
    
    float3 baseColor = texColor.rgb;
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    

    float3 litColor = baseColor * (ambient + diffuse) + specular;

    // 최종 출력용은 모든 효과를 포함
    //float3 finalColor = litColor + emissive;
    //finalColor = ApplyGlow(finalColor);
    //if (texColor.r > 0.3f)
    //{
    //    finalColor = ApplyHitFlash(finalColor,1.0f);        
    //}
    
    float4 finalColor = float4(litColor + emissive, texColor.a);
    finalColor.rgb = ApplyGlow(finalColor.rgb);

    //if (texColor.r > 0.3f)
    //{
    //    finalColor = ApplyHitFlash(finalColor); // 여기서 alpha도 같이 바뀜
    //}
    

    // output.c : 최종 결과
    output.c = float4(finalColor);

    // output.c1 : Blur 대상용 (보통 최종 결과와 동일하게 해도 무방)
    output.c1 = float4(finalColor);

    //output.c2 = float4(emissive, 1.0f); // 또는 threshold 기반으로 추출할 수도 있음
    //output.c2 = float4(1.f, 1.f, 1.f, 1.f); //float4(finalColor, texColor.a); // 또는 threshold 기반으로 추출할 수도 있음
    
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
    
    output.c4.rgb = diffuse;
    

    return output;
}