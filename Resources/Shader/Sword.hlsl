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
//    // �Ƿ翧 ������ ���
//    if (g_iRenderMode == 1)
//        return float4(0.2f, 0.2f, 0.2f, 1.0f);

//    // 1. UV �ְ�
//    float2 distortedUV = GetDistortedUV(input.t);

//    // 2. Dissolve Ŭ����
//    if (ShouldDissolve(distortedUV))
//        clip(-1);

//    // 3. �ؽ�ó ����
//    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
//    float3 baseColor = texColor.rgb;

//    // 4. ���� ���
//    float3 ambient = ApplyAmbient();
//    float3 diffuse = ApplyLambertLighting(input.n);
//    float3 specular = ApplySpecular(input.n, input.wPos);
//    float3 emissive = g_vEmissiveColor * g_fEmissivePower;

//    // 5. ���� ����
//    float3 litColor = baseColor * (ambient + diffuse) + specular;

//    // 6. Emissive ���� ����
//    litColor += emissive;

//    // 7. Glow, HitFlash �� ��ó�� ȿ��
//    litColor = ApplyGlow(litColor);
//    litColor = ApplyHitFlash(litColor);

//    // 8. ���
//    return float4(litColor, texColor.a);
//}

PS_OUT PS(VS_OUT_RIM input)
{
    PS_OUT output;

    // �Ƿ翧 ������ ���
    if (g_iRenderMode == 1)
    {
        output.color = float4(0.2f, 0.2f, 0.2f, 1.0f);
#ifdef USE_BLOOM
        output.bloom = float4(0, 0, 0, 1); // bloom ���
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
    output.bloom = float4(saturate(emissive), 1.0f); // bloom�� Emissive ���
#else
    output.c = float4(saturate(litColor), texColor.a);
#endif

    return output;
}