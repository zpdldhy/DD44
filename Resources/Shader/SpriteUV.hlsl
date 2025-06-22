#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};

//cbuffer CB_SpriteUV : register(b4)
//{
//    float2 g_uvStart;
//    float2 g_uvEnd;
//}

//float2 remapUV(float2 uv)
//{
//    return lerp(g_uvStart, g_uvEnd, uv);
//}

float3 ExtractScale(float4x4 m)
{
    float3 scale;
    scale.x = length(float3(m._11, m._12, m._13));
    scale.y = length(float3(m._21, m._22, m._23));
    scale.z = length(float3(m._31, m._32, m._33));

    return scale;
}

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // Camera ���� ���� (ViewMatrix�� �÷�����)
    float3 right = float3(g_matView._11, g_matView._21, g_matView._31);
    float3 up = float3(g_matView._12, g_matView._22, g_matView._32);
    
    float3 scale = ExtractScale(g_matWorld);

    // �簢�� �� ���� ��ġ (-0.5 ~ +0.5 ����)
    float3 offset = input.p.x * right * scale.x + input.p.y * up * scale.y;

    // ���� ���� ��ġ
    float3 instanceCenter = { g_matWorld._41, g_matWorld._42, g_matWorld._43 };
    float3 worldPos = instanceCenter + offset;

    // ��ȯ
    float4 viewPos = mul(float4(worldPos, 1), g_matView);
    float4 projPos = mul(viewPos, g_matProj);
    output.p = projPos; 

    output.c = input.c;
    output.n = input.n;
    output.t = input.t;

    return output;
}

VS_OUT VS_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    // Camera ���� ���� (ViewMatrix�� �÷�����)
    float3 right = float3(g_matView._11, g_matView._21, g_matView._31);
    float3 up = float3(g_matView._12, g_matView._22, g_matView._32);
    
    float3 scale = ExtractScale(input.matWorld);

    // �簢�� �� ���� ��ġ (-0.5 ~ +0.5 ����)
    float3 offset = input.p.x * right * scale.x + input.p.y * up * scale.y;

    // ���� ���� ��ġ
    float3 instanceCenter = { input.matWorld._41, input.matWorld._42, input.matWorld._43 };
    float3 worldPos = instanceCenter + offset;

    // ��ȯ
    float4 viewPos = mul(float4(worldPos, 1), g_matView);
    float4 projPos = mul(viewPos, g_matProj);
    output.p = projPos;

    output.c = input.c;
    output.n = input.n;
    output.t = lerp(input.uv.xy, input.uv.zw, input.t);

    return output;
}


PS_OUT_DUAL PS(VS_OUT input)
{
    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

    float2 uv = input.t;
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