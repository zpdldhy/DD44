#include "MainHeader.hlsli"

struct PS_OUT_UI
{
    float4 c : SV_Target0;
};

cbuffer CB_UVSlice : register(b2)
{
    float4 g_vUVSlice;
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
    
    float left = g_vUVSlice.x;
    float right = 1 - g_vUVSlice.y;
     
    float tiledX = input.t.x;
    
    // 왼쪽 경계를 반복하는 로직
    if (input.t.x > left && input.t.x < right)
    {
        float tileRegionWidth = right - left;
        float fracX = (input.t.x - left) / tileRegionWidth;
        
        if (fracX > 0.f &&fracX < 1.f)
            fracX = 0.f;
        
        tiledX = (fracX * tileRegionWidth) + left;
    }
    
    float top = g_vUVSlice.z;
    float bottom = 1 - g_vUVSlice.w;
    
    float tiledY = input.t.y;
    
    // 왼쪽 경계를 반복하는 로직
    if (input.t.y > top && input.t.y < bottom)
    {
        float tileRegionHieght = bottom - top;
        float fracY = (input.t.y - top) / tileRegionHieght;
        
        if (fracY > 0.f && fracY < 1.f)
            fracY = 0.f;
        
        tiledY = (fracY * tileRegionHieght) + top;
    }
    
    float2 tiledUV = float2(tiledX, tiledY);
    
    float4 texColor = g_txDiffuseA.Sample(sample, tiledUV);
    
    texColor.a *= input.c.a;
    psOut.c = texColor;
  
    return psOut;
}