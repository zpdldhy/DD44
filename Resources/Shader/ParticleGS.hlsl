#include "Header.hlsli"

// ������������������������������������������������������������������������������������������
// Vertex Shader Input
// ������������������������������������������������������������������������������������������

struct PARTICLE_VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float size : TEXCOORD0;
};

// ������������������������������������������������������������������������������������������
// Vertex Shader Output �� Geometry Shader Input
// ������������������������������������������������������������������������������������������

struct PARTICLE_VS_OUT
{
    float3 pos : POSITION; // ���� ��ġ (float4 �ƴ�!)
    float4 color : COLOR;
    float size : TEXCOORD0;
};

// ������������������������������������������������������������������������������������������
// Pixel Shader Input
// ������������������������������������������������������������������������������������������

struct PARTICLE_PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

// ������������������������������������������������������������������������������������������
// Vertex Shader
// ������������������������������������������������������������������������������������������

PARTICLE_VS_OUT VS(PARTICLE_VS_IN input)
{
    PARTICLE_VS_OUT output;
    output.pos = mul(float4(input.pos, 1.0f), g_matWorld).xyz; // ���� ��ǥ�� ���
    output.color = input.color;
    output.size = input.size;
    return output;
}

// ������������������������������������������������������������������������������������������
// Geometry Shader
// ������������������������������������������������������������������������������������������

[maxvertexcount(6)]
void GS(point PARTICLE_VS_OUT input[1], inout TriangleStream<PARTICLE_PS_IN> triStream)
{
    float3 center = input[0].pos;
    float size = input[0].size;

    float3 right = normalize(float3(g_matView._11, g_matView._21, g_matView._31));
    float3 up = normalize(float3(g_matView._12, g_matView._22, g_matView._32));

    float3 offset[4] =
    {
        (-right - up) * size * 0.5f,
        (right - up) * size * 0.5f,
        (-right + up) * size * 0.5f,
        (right + up) * size * 0.5f
    };

    float2 uvs[4] =
    {
        float2(0, 1), float2(1, 1),
        float2(0, 0), float2(1, 0)
    };

    PARTICLE_PS_IN v[4];
    for (int i = 0; i < 4; ++i)
    {
        float3 worldPos = center + offset[i];
        float4 viewPos = mul(float4(worldPos, 1.0f), g_matView);
        v[i].pos = mul(viewPos, g_matProj);
        v[i].color = input[0].color;
        v[i].uv = uvs[i];
    }

    // �ﰢ�� 1: v0, v1, v2
    triStream.Append(v[0]);
    triStream.Append(v[1]);
    triStream.Append(v[2]);

    // �ﰢ�� 2: v2, v1, v3
    triStream.Append(v[2]);
    triStream.Append(v[1]);
    triStream.Append(v[3]);
}

// ������������������������������������������������������������������������������������������
// Pixel Shader
// ������������������������������������������������������������������������������������������

float4 PS(PARTICLE_PS_IN input) : SV_Target
{
    float4 texColor = g_txDiffuseA.Sample(sample, input.uv);
    return texColor * input.color;
}