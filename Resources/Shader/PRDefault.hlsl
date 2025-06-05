#include "PRHeader.hlsli"

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

PS_OUT PS(VS_OUT input)
{
     
    PS_OUT psOut = (PS_OUT) 0;
    
    float2 uv = input.t;
    
    float4 original = g_txDefault.Sample(sample, input.t);
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

    float4 blur = g_txBlur.Sample(sample, uv) * weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i * g_fBlurScale, 0); // 陛煎 寞щ
        blur += g_txBlur.Sample(sample, uv + offset) * weights[i];
        blur += g_txBlur.Sample(sample, uv - offset) * weights[i];
    }
    
    float4 result = original;
    
    //if (blurMask > 0.5f)
    //    result = lerp(original, blur, 0.8f);

    //if (bloomMask > 0.5f && result.r > 0.5f)
    //    result.rgb += float3(1, 0.1, 0.1) * 0.7f; // 維釭朝 替釵 蹺陛

    //if (bloom.a > 0.01f)
    //    result.rgb += bloom.rgb * bloom.a * 2.0f;
    
    result.rgb = saturate(result.rgb);
   

    
// 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
// 縉雖 馬雖: Normal + Depth
// 式式式式式式式式式式式式式式式式式式式式式式式式式式式式式式
    float normalThreshold = 0.05f;
    float depthThreshold = 0.005f;

// Normal 晦奩 價Ы葭
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

// Depth 晦奩 價Ы葭
    float centerD = g_txDepth.Sample(sample, uv).r;
    float depthDiff = 0.0f;
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(-g_vTexelSize.x, 0)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(g_vTexelSize.x, 0)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(0, -g_vTexelSize.y)).r));
    depthDiff += step(depthThreshold, abs(centerD - g_txDepth.Sample(sample, uv + float2(0, g_vTexelSize.y)).r));

// 譆謙 縉雖 鬼紫: 譆渠 8 ⊥ 0~1 彰嬪 薑敘��
    float edge = saturate((normalDiff + depthDiff) / 8.0f);

// 縉雖 轎溘
   
    
    switch (g_iDebugMode)
    {
        case 0:
            {
                float edgeAlpha = smoothstep(0.05f, 0.2f, edge); // 縉雖 馬雖 鬼紫
                float3 finalColor = result.rgb * (1.0f + edgeAlpha * 0.5f); // 嫩晦虜 髦礎 隸陛
                psOut.c.rgb = saturate(finalColor); // 儀 彰嬪 贗極ё
                psOut.c.a = input.c.a;
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