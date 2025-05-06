#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;

    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, g_matWorld);

    float4x4 viewNoTrans = g_matView;
    viewNoTrans._41 = 0;
    viewNoTrans._42 = 0;
    viewNoTrans._43 = 0;

    float4 vView = mul(vWorld, viewNoTrans);
    float4 vProj = mul(vView, g_matProj);

    output.p = vProj.xyzz;
    output.c = input.c;
    output.n = input.n;
    output.t = input.t;
    
    return output;
}


PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;
    
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    psOut.c = texColor;
    
    return psOut;
}
