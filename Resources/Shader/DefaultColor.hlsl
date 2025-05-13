#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    
    float4x4 projNoNearFar = g_matProj;
    projNoNearFar._33 = 1.0f;
    projNoNearFar._43 = -0.1f;
    
    float4 vProj = mul(vView, projNoNearFar);
    
    output.p = vProj;
    output.c = input.c;
    output.n = input.n;
    output.t = input.t;
    
    return output;
}

PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;
    psOut.c = input.c;
    psOut.c1 = input.c;
    psOut.c2 = input.c;
    return psOut;
}