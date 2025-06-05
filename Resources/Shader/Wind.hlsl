#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    // 월드 → 뷰 → 프로젝션 제거
    float2 ndcOffset = g_matWorld._41_42; // 위치값만 사용
    float2 ndcPos = input.p.xy + ndcOffset;

    output.p = float4(ndcPos, 0.0f, 1.0f); // NDC로 직접 출력
    output.t = input.t * g_vUVSize + g_vUVStart + g_vUVOffset;

    return output;
}

float4 PS(VS_OUT input) : SV_Target
{
    return g_txDiffuseA.Sample(sample, input.t);

}