#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld); // �� ����
    output.p = mul(mul(worldPos, g_matView), g_matProj); // �״�� ����

    output.t = input.t;
    return output;
}

PS_OUT_DUAL PS(VS_OUT input)
{
    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    if (texColor.a < 0.1f)
        discard;

    float alpha = max(max(texColor.r, texColor.g), texColor.b);

    psOut.c0 = float4(texColor.rgb, texColor.a);
    psOut.c1 = float4(1.0f - texColor.rgb, alpha); // Dual Blend�� ������

    return psOut;
}




//VS_OUT VS(VS_IN input)
//{
//    VS_OUT output = (VS_OUT) 0;

//    // 1. ���� ��ǥ (���� ����)
//    float3 localPos = float3(input.p.x * g_vBillboardSize.x,
//                             input.p.y * g_vBillboardSize.y,
//                             0.0f); // �׻� flat�� plane�̶�� ����

//    // 2. ȸ���� ��ǥ ��� (���� ��Ʈ������ ȸ�� ����)
//    float4 worldPos = mul(float4(localPos, 1.0f), g_matWorld);

//    // 3. ���� ��ġ�� g_vBillboardCenter �������� ����
//   // worldPos.xyz += g_vBillboardCenter;

//    // 4. ��/���� ��ȯ
//    float4 viewPos = mul(worldPos, g_matView);
//    output.p = mul(viewPos, g_matProj);

//    // 5. ��Ÿ ����
//    output.c = input.c;
//    output.n = input.n;
//    output.t = remapUV(input.t); // ���⿡ ȸ���� UV �־ ��

//    return output;
//}

//PS_OUT_DUAL PS(VS_OUT input)
//{
//    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

//    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    
//    if(texColor.a < 0.1)
//        discard;

//    float alpha = max(max(texColor.r, texColor.g), texColor.b);

//    // SV_Target0 �� ���� �� �״��
//    psOut.c0 = float4(texColor.rgb, 1.0f);

//    // SV_Target1 �� ����� ���� ���� ��
//    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha);

//    return psOut;
//}


//PS_OUT_DUAL PS(VS_OUT input)
//{
//    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

//    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    
//    if (texColor.a < 0.1)
//        discard;

//    float alpha = max(max(texColor.r, texColor.g), texColor.b);

//    // SV_Target0 �� ���� �� �״��
//    psOut.c0 = float4(texColor.rgb, 1.0f);

//    // SV_Target1 �� ����� ���� ���� ��
//    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha);

//    return psOut;
//}
