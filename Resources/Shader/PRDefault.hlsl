#include "MainHeader.hlsli"

struct PS_PROCESS_OUT
{
    float4 c : SV_Target;
};

cbuffer CB_Debug : register(b3)
{
    int g_iDebugMode;
    float3 padding_Debug;
}

cbuffer CB_Blur : register(b11)
{
    float2 g_vTexelSize; // (1 / 화면 너비, 1 / 화면 높이)
    float2 g_vDirection; // (1, 0): 가로 블러 / (0, 1): 세로 블러
    float g_fBlurScale;
    float3 padding_blur;
};

Texture2D g_txBlur : register(t1); // Blur
Texture2D g_txMask : register(t2); // Mask
Texture2D g_txBloom : register(t3); // Bloom
Texture2D g_txLight : register(t4); // Light
Texture2D g_txNormal : register(t5); // Normal
Texture2D g_txDepth : register(t6); // Depth
Texture2D g_txTemp : register(t7); // Temp

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
    
    output.p = vProj;
    output.c = input.c;
    output.n = input.n;
    output.t = input.t;
    
    return output;
}

PS_PROCESS_OUT PS(VS_OUT input)
{     
    PS_PROCESS_OUT psOut = (PS_PROCESS_OUT) 0;
    
    float2 uv = input.t;
    
    float4 original = g_txDiffuseA.Sample(sample, input.t);
    float blurMask = g_txMask.Sample(sample, input.t).r;
    float bloomMask = g_txMask.Sample(sample, input.t).g;
    float4 bloom = g_txBloom.Sample(sample, input.t);
    float4 light = g_txLight.Sample(sample, input.t);
    float4 normal = g_txNormal.Sample(sample, input.t);
    float4 wind = g_txTemp.Sample(sample, input.t);
    
    float weights[5] =
    {
        0.227027f, 0.1945946f, 0.1216216f, 0.0540541f, 0.0162162f
    };

    float4 blurH = g_txBloom.Sample(sample, uv) * weights[0];
    float4 blurV = g_txBloom.Sample(sample, uv) * weights[0];
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = g_vTexelSize * i * g_fBlurScale * float2(1,0);

        blurH += g_txBloom.Sample(sample, uv + offset) * weights[i];
        blurH += g_txBloom.Sample(sample, uv - offset) * weights[i];
    }
    
    for (int j = 1; j < 5; ++j)
    {
        float2 offset = g_vTexelSize * i * g_fBlurScale * float2(0, 1);

        blurV += g_txBloom.Sample(sample, uv + offset) * weights[j];
        blurV += g_txBloom.Sample(sample, uv - offset) * weights[j];
    }
    
    float4 blur = blurH + blurV;
   
    
    
    float4 result = original;
    
    //if (blurMask > 0.5f)
    //    result = lerp(original, blur, 0.8f);

    //if (bloomMask > 0.5f && result.r > 0.5f)
    //    result.rgb += float3(1, 0.1, 0.1) * 0.7f; // 빛나는 느낌 추가

    //if (bloom.a > 0.01f)
    //    result.rgb += bloom.rgb * bloom.a * 2.0f;
    
    //result.rgb = saturate(result.rgb);
   

    if (bloom.r > 0.01f)
    {
        float bloomIntensity = 1.f; // 필요 시 조절
        result.rgb += blur.rgb * bloomIntensity; // * bloomIntensity;
    }
    
// ──────────────────────────────
// 엣지 감지: Normal + Depth
// ──────────────────────────────
    float normalThreshold = 0.05f;
    float depthThreshold = 0.005f;

// Normal 기반 샘플링
    float3 centerN = g_txNormal.Sample(sample, uv).rgb;
    float3 leftN = g_txNormal.Sample(sample, uv + float2(-g_vTexelSize.x, 0)).rgb;
    float3 rightN = g_txNormal.Sample(sample, uv + float2(g_vTexelSize.x, 0)).rgb;
    float3 upN = g_txNormal.Sample(sample, uv + float2(0, -g_vTexelSize.y)).rgb;
    float3 downN = g_txNormal.Sample(sample, uv + float2(0, g_vTexelSize.y)).rgb;

    float normalDiff = 0.0f;
    normalDiff += step(normalThreshold, length(centerN - leftN));
    normalDiff += step(normalThreshold, length(centerN - rightN));
    normalDiff += step(normalThreshold, length(centerN - upN));
    normalDiff += step(normalThreshold, length(centerN - downN));

// Depth 기반 샘플링
    float centerD = g_txDepth.Sample(sample, uv).r;
    float depthDiff = 0.0f;
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(-g_vTexelSize.x, 0)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(g_vTexelSize.x, 0)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(0, -g_vTexelSize.y)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(0, g_vTexelSize.y)).r));

// 최종 엣지 강도: 최대 8 → 0~1 범위 정규화
    float edge = saturate((normalDiff + depthDiff) / 8.0f);

// 엣지 출력
   
    
    switch (g_iDebugMode)
    {
        case 0:
            {
                float edgeAlpha = smoothstep(0.05f, 0.2f, edge); // 엣지 감지 강도
                float3 baseColor = result.rgb * (1.0f + edgeAlpha * 0.5f);
                float3 finalColor = lerp(baseColor, wind.rgb, wind.a);

                psOut.c.rgb = saturate(finalColor);
                psOut.c.a = input.c.a; // 알파는 원본 그대로 유지
                break;
            }
        case 1:
            psOut.c = original;
            break;
        case 2:
            psOut.c = blur;
            break;
        case 3:
            psOut.c = float4(blurMask, 0, 0, 1);
            break;
        case 4:
            psOut.c = bloom;
            break;
        case 5:
            psOut.c = float4(0, bloomMask, 0, 1);
            break;
        case 6:
            psOut.c = light;
            break;
        case 7:
            psOut.c = normal;
            break;
        case 8:
            psOut.c.rgb = (edge > 0.2f) ? float3(1, 1, 1) : float3(0, 0, 0);
            psOut.c.a = 1.0f;
            break;
        default:
            psOut.c = result;
            break;
    }
    psOut.c.a = input.c.a;
    

    return psOut;
}