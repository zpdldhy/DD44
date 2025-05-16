#include "PRHeader.hlsli"


//Texture2D HitFlashTex : register(t0); // ����

// Merge�� Vertex Shader
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

// Merge�� Pixel Shader
PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;

    float4 hitFlash = g_txDefault.Sample(sample, input.t);

    //// ���� ���� �ʹ� ������ ���� (optional)
    //if (hitFlash.a < 0.01f)
    //    discard;

    psOut.c = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //hitFlash; // �״�� AddBlend�� ���
    psOut.c.a = 1.0f;
    //input.c.a;
    return psOut;
}