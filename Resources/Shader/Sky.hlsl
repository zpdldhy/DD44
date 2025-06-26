#include "Header.hlsli"

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 vLocal = float4(input.p, 1.0f);
    float4 vWorld = mul(vLocal, g_matWorld);

    float4x4 viewNoTrans = g_matView;
    viewNoTrans._41 = 0;
    viewNoTrans._42 = 0;
    viewNoTrans._43 = 0;
    
    float4x4 projNoNearFar = g_matProj;
    projNoNearFar._33 = 1.0f;
    projNoNearFar._43 = -0.1f;

    float4 vView = mul(vWorld, viewNoTrans);
    float4 vProj = mul(vView, projNoNearFar);

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

    float speed = 0.02;
    float t = fmod(g_fGameTime * speed, 1.0);

    // 기본 HSV
    float h = 0.0;
    float s = 1.0;
    float v = 1.0;

    // 구간별 조건 처리
    if (t < 0.2 || t >= 0.6)
    {
        // 낮
        h = 0.0;
        s = 1.0;
        v = 1.0;
    }
    else if (t < 0.3)
    {
        // 노을 (0.2 ~ 0.3)
        float k = (t - 0.2) / 0.1; // 0~1
        h = lerp(0.0, 30.0, k);
        s = lerp(1.0, 5.0, k);
        v = lerp(1.0, 0.4, k);
    }
    else if (t < 0.5)
    {
        // 밤 (0.3 ~ 0.5)
        float k = (t - 0.3) / 0.2;
        h = 30.0;
        s = lerp(5.0, 0.4, k);
        v = 0.4;
    }
    else if (t < 0.6)
    {
        // 새벽 (0.5 ~ 0.6)
        float k = (t - 0.5) / 0.1;
        h = lerp(30.0, 0.0, k);
        s = lerp(0.4, 1.0, k);
        v = lerp(0.4, 1.0, k);
    }

    psOut.c.xyz = clamp(TransformHSV(psOut.c.xyz, h, s, v), 0.0, 1.0);
   
    return psOut;
}
