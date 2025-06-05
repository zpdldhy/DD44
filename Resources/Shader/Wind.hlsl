#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    // ���� �� �� �� �������� ����
    float2 ndcOffset = g_matWorld._41_42; // ��ġ���� ���
    float2 ndcPos = input.p.xy + ndcOffset;

    output.p = float4(ndcPos, 0.0f, 1.0f); // NDC�� ���� ���
    output.t = input.t * g_vUVSize + g_vUVStart + g_vUVOffset;

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    return g_txDiffuseA.Sample(sample, input.t);

}