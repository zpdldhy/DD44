#include "PRHeader.hlsli"


//Texture2D HitFlashTex : register(t0); // 원본

// Merge용 Vertex Shader
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

// Merge용 Pixel Shader
PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;

    float4 hitFlash = g_txDefault.Sample(sample, input.t);

    //// 알파 값이 너무 낮으면 무시 (optional)
    //if (hitFlash.a < 0.01f)
    //    discard;

    psOut.c = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //hitFlash; // 그대로 AddBlend로 출력
    psOut.c.a = 1.0f;
    //input.c.a;
    return psOut;
}