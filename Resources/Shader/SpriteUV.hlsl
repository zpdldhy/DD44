#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // [1] View 행렬에서 Right(X), Up(Y) 벡터 추출
    float3 right = float3(g_matView._11, g_matView._21, g_matView._31);
    float3 up = float3(g_matView._12, g_matView._22, g_matView._32);

    
    float3 offset = (input.p.x * right * g_vBillboardSize.x) +
                (input.p.y * up * g_vBillboardSize.y);

    float3 worldPos = g_vBillboardCenter + offset;

    // [3] View/Proj 적용
    float4 viewPos = mul(float4(worldPos, 1.0f), g_matView);
    output.p = mul(viewPos, g_matProj);

    output.c = input.c;
    output.n = input.n;
    output.t = input.t;

    return output;
}

PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;

    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    psOut.c = texColor;

    return psOut;
}