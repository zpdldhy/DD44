#include "Header.hlsli"

struct VS_PARTICLE_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float size : TEXCOORD0;
};

struct VS_PARTICLE_OUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float size : TEXCOORD0;
};

VS_PARTICLE_OUT VS(VS_PARTICLE_IN input)
{
    VS_PARTICLE_OUT output;
    output.pos = mul(float4(input.pos, 1.0f), g_matWorld).xyz;
    output.color = input.color;
    output.size = input.size;
    return output;
}

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

[maxvertexcount(6)]
void GS(point VS_PARTICLE_OUT input[1], inout TriangleStream<PS_IN> triStream)
{
    float3 center = input[0].pos;
    float size = input[0].size;
    float4 color = input[0].color;

    float3 right = normalize(float3(g_matView._11, g_matView._21, g_matView._31));
    float3 up = normalize(float3(g_matView._12, g_matView._22, g_matView._32));

    float3 offsets[4] =
    {
        -right * size + up * size,
         right * size + up * size,
        -right * size - up * size,
         right * size - up * size
    };

    float2 uvs[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };

    int indices[6] = { 0, 1, 2, 2, 1, 3 };

    for (int i = 0; i < 6; ++i)
    {
        PS_IN vOut;
        float3 finalPos = center + offsets[indices[i]];
        vOut.pos = mul(mul(float4(finalPos, 1.0f), g_matView), g_matProj);
        vOut.uv = uvs[indices[i]];
        vOut.color = color;
        triStream.Append(vOut);
    }
}

float4 PS(PS_IN input) : SV_Target
{
    float4 texColor = g_txDiffuseA.Sample(sample, input.uv);
    return texColor * input.color;
}