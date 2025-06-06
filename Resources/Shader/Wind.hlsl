
#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    // NDC�� ���� ��ġ ��ȯ
    float4 local = float4(input.p.xy, 0.0f, 1.0f);
    output.p = mul(local, g_matWorld); // ������, ��ġ �ݿ�
    output.t = input.t; // UV �״�� ���

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    float4 result = g_txDiffuseA.Sample(sample, input.t);
    if (result.a < 0.4f)
    {
        discard;
    }
    
    return result;
}
