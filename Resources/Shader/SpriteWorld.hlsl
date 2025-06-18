#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.p = mul(viewPos, g_matProj);
    output.t = input.t;

    return output;
}

VS_OUT VS_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.p, 1.0f), input.matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.p = mul(viewPos, g_matProj);
    output.t = input.t;

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    float2 uv = input.t;
    float4 texColor = g_txDiffuseA.Sample(sample, uv);

    if (texColor.a < 0.1f)
        discard;

    // �߽����κ��� �Ÿ� ��� (0 = �߽�, 1 = �ٱ�)
    float distFromCenter = distance(uv, float2(0.5f, 0.5f));
    float edgeFactor = saturate(distFromCenter * 2.0f);

    //// �߽��� ���, �ܰ��� ���� Ǫ���� ����
    //float3 centerColor = float3(1.0f, 1.0f, 1.0f);
    //float3 outerTint = float3(0.6f, 0.8f, 1.0f); // ���� �Ķ�

    //float3 finalColor = lerp(centerColor, outerTint, edgeFactor);
    //texColor.rgb *= finalColor;

    // ���� ���� ���̱�
    float alphaFalloff = 1.0f - edgeFactor * 0.5f;
    texColor.a *= alphaFalloff;

    return texColor;
}