#include "MainHeader.hlsli"

struct PS_OUT_UI
{
    float4 c : SV_Target0;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
    
    output.p = vProj;
    output.c = float4(0.f, 0.f, 0.f, 0.f);
    output.n = input.n;
    output.t = input.t;
    
    return output;
}

VS_OUT VS_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output;
    
    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, input.matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
    
    output.p = vProj;
    output.c = input.color;
    output.n = input.n;
    output.t = lerp(input.uv.xy, input.uv.zw, input.t);
    
    return output;
}

PS_OUT_UI PS(VS_OUT input)
{
    PS_OUT_UI psOut = (PS_OUT_UI) 0;
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    
    texColor += input.c;
    texColor = float4(ApplyGlow(texColor.rgb), texColor.a);
    
    psOut.c = texColor;
  
    return psOut;
}