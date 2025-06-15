#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    float4 viewPos = mul(worldPos, g_matView);
    output.p = mul(viewPos, g_matProj);
    output.t = input.t;
    if (reverse)
    {
        output.t.x = 1.0f - input.t.x;
    }

    return output;
}



float4 PS(VS_OUT input) : SV_Target
{
    float x = input.t.x;
    float size = 0.7f;
    float offset = 0.2f;
    float start;
    float end;
    
    if (g_fProgress < size)
    {
        start = offset;
        end = g_fProgress + offset;
    }
    else
    {
        start = offset + g_fProgress - size;
        end = offset + g_fProgress;
    }
    

    if (x < start || x > end)
        discard;
    
    float4 color = g_txDiffuseA.Sample(sample, input.t);
    
    // 1. ��� ��� (Luminance)
    float luminance = dot(color.rgb, float3(0.3, 0.59, 0.11));

    // 2. ��ī�ο� ���� ����ũ ����
    float edgeMask = smoothstep(0.1, 0.2, luminance);

    // 3. Glow ���� ���� (�ణ ���� �����)
    float3 glowColor = float3(1.0, 0.3, 0.2);

    // 4. ���� �÷��� glow ���� (���� ������)
    float3 finalColor = lerp(color.rgb, glowColor, edgeMask * 0.8);

    // 5. ��Ʈ��Ʈ ��ȭ
    finalColor = pow(finalColor, float3(0.8, 0.8, 0.8));

    // 6. ���ĸ� ��ī�ο� ��迡�� �츮��
    float alpha = edgeMask;
    
    return float4(finalColor, alpha);
}