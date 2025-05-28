#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};


VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;


    float3 right = float3(g_matView._11, g_matView._21, g_matView._31);
    float3 up = float3(g_matView._12, g_matView._22, g_matView._32);

    
    float3 offset = (input.p.x * right * g_vBillboardSize.x) +
                (input.p.y * up * g_vBillboardSize.y);

    float3 worldPos = g_vBillboardCenter + offset;

    float4 viewPos = mul(float4(worldPos, 1.0f), g_matView);
    output.p = mul(viewPos, g_matProj);

    output.c = input.c;
    output.n = input.n;
    output.t = input.t;

    return output;
}

PS_OUT_DUAL PS(VS_OUT input)
{
    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

    float2 uv = remapUV(input.t);
    float4 texColor = g_txDiffuseA.Sample(sample, uv);

    if (texColor.a < 0.1)
        discard;

    // [1] �߽� �Ÿ� ��� edge factor ���
    float distFromCenter = distance(uv, float2(0.5f, 0.5f));
    float edgeFactor = saturate(distFromCenter * 2.0f); // 0: �߽�, 1: �ܰ�

    // [2] �߽��� ���, �ٱ��� g_vTintColor�� ����
    float3 innerColor = float3(1.0f, 1.0f, 1.0f); // ��� �߽�
    float3 tint = lerp(innerColor, g_vTintColor.rgb, edgeFactor);

    texColor.rgb *= tint;

    // [3] ���� ���� - �ٱ����� ������ �帮��
    float alphaFalloff = 1.0f - edgeFactor * 0.5f; // �ٱ��ʿ��� ���� ���̱�
    texColor.a *= alphaFalloff;

    float alpha = max(max(texColor.r, texColor.g), texColor.b);

    // [4] Output to render targets
    psOut.c0 = float4(texColor.rgb, texColor.a);
    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha);

    return psOut;
}

//PS_OUT_DUAL PS(VS_OUT input)
//{
//    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

//    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    
//    if(texColor.a < 0.1)
//        discard;

//    texColor.rgb *= g_vTintColor.rgb;
    
//    float alpha = max(max(texColor.r, texColor.g), texColor.b);

//    // SV_Target0 �� ���� �� �״��
//    psOut.c0 = float4(texColor.rgb, 1.0f);

//    // SV_Target1 �� ����� ���� ���� ��
//    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha);

//    return psOut;
//}