Texture2D g_txScene : register(t0); // 원본 화면 텍스처
Texture2D g_txBloom : register(t1); // 블룸 텍스처
SamplerState sampleState : register(s0);

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

float4 PS(VS_OUT input) : SV_Target
{
    float4 sceneColor = g_txScene.Sample(sampleState, input.uv);
    float4 bloomColor = g_txBloom.Sample(sampleState, input.uv);

    // 블룸 강도 조절 가능
    float bloomStrength = 1.0f;

    float4 result = sceneColor + bloomColor * bloomStrength;

    return saturate(result); // 과도한 밝기 방지
}