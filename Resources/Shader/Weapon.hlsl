#include "Header.hlsli"

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
    
    if (g_iRenderMode == 1)
    {
        float4 silhouette = float4(0.2f, 0.2f, 0.2f, 1.0f);
        psOut.c = silhouette;
        psOut.c1 = silhouette;
        psOut.c2 = silhouette;
        return psOut;
    }
    
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    float dissolve = g_txNoise.Sample(sample, input.t).r;
    
    if (dissolve < g_fDissolveAmount)
        discard;
    texColor.a *= input.c.a;
    psOut.c = texColor;

  
    return psOut;
}