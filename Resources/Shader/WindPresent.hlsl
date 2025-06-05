#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 local = float4(input.p.xy, 0.0f, 1.0f); // �⺻ Plane vertex (-0.5 ~ +0.5)
    float4 world = mul(local, g_matWorld); // ���� ��ȯ (������+��ġ ����)

    output.p = float4(world.xy, 0.0f, 1.0f); //  �״�� NDC ���
    output.t = input.t;
    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    return g_txDiffuseA.Sample(sample, input.t);
}