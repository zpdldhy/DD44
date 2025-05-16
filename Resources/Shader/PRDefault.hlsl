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
    
    float weights[5] =
    {
        0.227027f, 0.1945946f, 0.1216216f, 0.0540541f, 0.0162162f
    };

    float4 blur = g_txBlur.Sample(sample, uv) * weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i * g_fBlurScale, 0); // °¡·Î ¹æÇâ
        blur += g_txBlur.Sample(sample, uv + offset) * weights[i];
        blur += g_txBlur.Sample(sample, uv - offset) * weights[i];
    }
    
    float4 result = original;
    
    if (blurMask > 0.5f)
        result = lerp(original, blur, 0.8f);

    if (bloomMask > 0.5f && result.r > 0.5f)
        result.rgb += float3(1, 0.1, 0.1) * 0.7f; // ºû³ª´Â ´À³¦ Ãß°¡

    if (bloom.a > 0.01f)
        result.rgb += bloom.rgb * bloom.a * 2.0f;
    
    result.rgb = saturate(result.rgb);
   
    psOut.c = result;
    psOut.c.a = input.c.a;
    
    //test.a = 1.0f;
    
    
    //float4 result;
    if(0)
    {
        psOut.c = normal;
        psOut.c.a = 1.f;
        // Normal ±â¹Ý »ùÇÃ
        float3 centerN = g_txNormal.Sample(sample, uv).rgb;
        float3 leftN = g_txNormal.Sample(sample, uv + float2(-g_vTexelSize.x, 0)).rgb;
        float3 rightN = g_txNormal.Sample(sample, uv + float2(g_vTexelSize.x, 0)).rgb;
        float3 upN = g_txNormal.Sample(sample, uv + float2(0, -g_vTexelSize.y)).rgb;
        float3 downN = g_txNormal.Sample(sample, uv + float2(0, g_vTexelSize.y)).rgb;

        float normalDiff = 0.0f;
        normalDiff += length(centerN - leftN);
        normalDiff += length(centerN - rightN);
        normalDiff += length(centerN - upN);
        normalDiff += length(centerN - downN);

// Depth ±â¹Ý »ùÇÃ
        float centerD = g_txDepth.Sample(sample, uv).r;
        float leftD = g_txDepth.Sample(sample, uv + float2(-g_vTexelSize.x, 0)).r;
        float rightD = g_txDepth.Sample(sample, uv + float2(g_vTexelSize.x, 0)).r;
        float upD = g_txDepth.Sample(sample, uv + float2(0, -g_vTexelSize.y)).r;
        float downD = g_txDepth.Sample(sample, uv + float2(0, g_vTexelSize.y)).r;

        float depthDiff = 0.0f;
        depthDiff += abs(centerD - leftD);
        depthDiff += abs(centerD - rightD);
        depthDiff += abs(centerD - upD);
        depthDiff += abs(centerD - downD);

// ÃÖÁ¾ ¿§Áö °¨Áö°ª
        float edge = saturate((normalDiff + depthDiff * 10.0f) * 5.0f); // depth´Â °¨µµ ³ôÀÌ±â À§ÇØ *10

// ¿§Áö Ãâ·Â
        if (edge > 0.2f)
        {
            psOut.c.rgb = float3(1, 1, 1); // À±°û¼± Èò»ö
            psOut.c.a = 1.0f;
        }
        else
        {
            psOut.c = result;
            psOut.c.a = input.c.a;
        }

    }
    
    
    
    return psOut;
}