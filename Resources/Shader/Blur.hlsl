cbuffer CB_Blur : register(b3)
{
    float2 g_vTexelSize; // (1 / 화면 너비, 1 / 화면 높이)
    float2 g_vDirection; // (1, 0): 가로 블러 / (0, 1): 세로 블러
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    output.pos = float4(input.pos, 1.0f);
    output.uv = input.uv;
    return output;
}

Texture2D g_txInput : register(t0);
SamplerState sampleState : register(s0);

float weights[5] = { 0.204164f, 0.304005f, 0.093913f, 0.017459f, 0.001841f };

float4 PS(VS_OUT input) : SV_Target
{
    float2 uv = input.uv;
    float4 color = g_txInput.Sample(sampleState, uv) * weights[0];

    for (int i = 1; i < 5; ++i)
    {
        float2 offset = g_vTexelSize * g_vDirection * float(i);
        color += g_txInput.Sample(sampleState, uv + offset) * weights[i];
        color += g_txInput.Sample(sampleState, uv - offset) * weights[i];
    }

    return color;
}