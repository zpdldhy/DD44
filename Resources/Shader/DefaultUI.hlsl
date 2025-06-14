#include "MainHeader.hlsli"

struct PS_OUT_UI
{
    float4 c : SV_Target0;
};

cbuffer CB_UVSlice : register(b3)
{
    float4 g_Color;
}

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

PS_OUT_UI PS(VS_OUT input)
{     
    PS_OUT_UI psOut = (PS_OUT_UI) 0;    
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    
    texColor += g_Color;    
    texColor = float4(ApplyGlow(texColor.rgb), texColor.a);
    
    texColor.a *= input.c.a;
    psOut.c = texColor;
  
    return psOut;
}