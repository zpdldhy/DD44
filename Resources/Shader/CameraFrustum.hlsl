#include "MainHeader.hlsli"

cbuffer CB_Frustum : register(b3)
{
    row_major matrix g_matViewProjInvert;
}

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 vNDC = float4(input.p, 1.0f);
    float4 vWorld = mul(vNDC, g_matViewProjInvert);
    //vWorld = mul(vWorld, g_matWorld);
    
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

    return psOut;
}