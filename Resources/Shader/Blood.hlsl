#include "Header.hlsli"

struct PS_OUT_DUAL
{
    float4 c0 : SV_Target0;
    float4 c1 : SV_Target1;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld); // ← 수정
    output.p = mul(mul(worldPos, g_matView), g_matProj); // 그대로 유지

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
    psOut.c1 = float4(1.0f - texColor.rgb, alpha); // Dual Blend용 보조색

    return psOut;
}




//VS_OUT VS(VS_IN input)
//{
//    VS_OUT output = (VS_OUT) 0;

//    // 1. 쿼드 좌표 (로컬 공간)
//    float3 localPos = float3(input.p.x * g_vBillboardSize.x,
//                             input.p.y * g_vBillboardSize.y,
//                             0.0f); // 항상 flat한 plane이라고 가정

//    // 2. 회전된 좌표 계산 (월드 매트릭스에 회전 포함)
//    float4 worldPos = mul(float4(localPos, 1.0f), g_matWorld);

//    // 3. 실제 위치는 g_vBillboardCenter 기준으로 조정
//   // worldPos.xyz += g_vBillboardCenter;

//    // 4. 뷰/투영 변환
//    float4 viewPos = mul(worldPos, g_matView);
//    output.p = mul(viewPos, g_matProj);

//    // 5. 기타 전달
//    output.c = input.c;
//    output.n = input.n;
//    output.t = remapUV(input.t); // 여기에 회전된 UV 넣어도 됨

//    return output;
//}

//PS_OUT_DUAL PS(VS_OUT input)
//{
//    PS_OUT_DUAL psOut = (PS_OUT_DUAL) 0;

//    float4 texColor = g_txDiffuseA.Sample(sample, remapUV(input.t));
    
//    if(texColor.a < 0.1)
//        discard;

//    float alpha = max(max(texColor.r, texColor.g), texColor.b);

//    // SV_Target0 → 원본 색 그대로
//    psOut.c0 = float4(texColor.rgb, 1.0f);

//    // SV_Target1 → 감쇄용 보조 블렌딩 값
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

//    // SV_Target0 → 원본 색 그대로
//    psOut.c0 = float4(texColor.rgb, 1.0f);

//    // SV_Target1 → 감쇄용 보조 블렌딩 값
//    psOut.c1 = float4(1.0f - alpha, 1.0f - alpha, 1.0f - alpha, alpha);

//    return psOut;
//}
