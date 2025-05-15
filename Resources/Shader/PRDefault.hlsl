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
    float4 bloom = g_txBloom.Sample(sample, input.t);
    
    float weights[5] =
    {
        0.227027f, 0.1945946f, 0.1216216f, 0.0540541f, 0.0162162f
    };

    float4 blur = g_txBlur.Sample(sample, uv) * weights[0];
    
    for (int i = 1; i < 5; ++i)
    {
        float2 offset = float2(g_vTexelSize.x * i * g_fBlurScale , 0); // 가로 방향
        blur += g_txBlur.Sample(sample, uv + offset) * weights[i];
        blur += g_txBlur.Sample(sample, uv - offset) * weights[i];
    }
    
    psOut.c = (blur + original)/2;
    psOut.c.a = 1.0f;
//    input.c.a;
    return psOut;
}