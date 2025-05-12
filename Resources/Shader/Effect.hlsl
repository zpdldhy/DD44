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

float4 PS(VS_OUT_RIM input) : SV_Target
{
    // �Ƿ翧 ������ ���
    if (g_iRenderMode == 1)
        return float4(0.2f, 0.2f, 0.2f, 1.0f);

    // 1. UV �ְ�
    float2 distortedUV = GetDistortedUV(input.t);

    // 2. Dissolve Ŭ����
    if (ShouldDissolve(distortedUV))
        clip(-1);

    // 3. �ؽ�ó ����
    float4 texColor = g_txDiffuseA.Sample(sample, distortedUV);
    float3 baseColor = texColor.rgb;

    // 4. ���� ���
    float3 ambient = ApplyAmbient();
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 emissive = g_vEmissiveColor * g_fEmissivePower;

    // 5. ���� ����
    float3 litColor = baseColor * (ambient + diffuse) + specular;

    // 6. Emissive ���� ����
    litColor += emissive;

    // 7. Glow, HitFlash �� ��ó�� ȿ��
    litColor = ApplyGlow(litColor);
    litColor = ApplyHitFlash(litColor);

    // 8. ���
    return float4(litColor, texColor.a);
    
}