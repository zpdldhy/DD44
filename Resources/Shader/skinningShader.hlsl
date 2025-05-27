#include "Header.hlsli"

VS_OUT_RIM VS(PNCTIW_IN input)
{
    VS_OUT_RIM output;
    output.p = float4(input.p, 1.0f);
    float4 vLocal = float4(input.p, 1.0f);
    
    float4 vAnim;
       
    for (int i = 0; i < 4; i++)
    {
        int iBoneIndex = (int) input.i[i];
        float fWeight = input.w[i];
        
        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = obj_matAnim[iBoneIndex];
        
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    for (int j = 0; j < 4; j++)
    {
        int iBoneIndex = (int) input.i2[j];
        float fWeight = input.w2[j];

        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = obj_matAnim[iBoneIndex];
        
        animMat = mul(boneMat, animMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    float4 vWorld = mul(vAnim, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
        
    output.p = vProj;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.wPos = worldPos.xyz;
    output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    
    output.c = input.c;
    //output.n = input.n;
    output.t = input.t;
    
    return output;
}

//PS_OUT PS(VS_OUT input)
//{
//    PS_OUT psOut = (PS_OUT) 0;
//    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
//    texColor.a *= input.c.a;
//    psOut.c = texColor;
//    //psOut.c = float4(input.n, 1.0f);
//    return psOut;
//}

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
        emissive = g_vEmissiveColor * g_fEmissivePower * 5.0f;
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