#include "Header.hlsli"

Texture2D g_txScene : register(t0); // 원본 씬
Texture2D g_txBlur : register(t1); // 블러 결과
SamplerState g_samp : register(s0);

struct COMBINE_VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct COMBINE_PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

COMBINE_PS_IN VS(COMBINE_VS_IN input)
{
    COMBINE_PS_IN output;
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    return output;
}

float4 PS(COMBINE_PS_IN input) : SV_Target
{
    float4 sceneColor = g_txScene.Sample(g_samp, input.uv);
    float4 blurColor = g_txBlur.Sample(g_samp, input.uv);

    // 간단한 가중치 조합: 원본 + 블러 강조
    float4 result = sceneColor + blurColor * 0.5f;
    //return saturate(result);
    return float4(1.f, 0.f, 0.f, 1.f);

}
