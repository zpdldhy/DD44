#define MAX_BONE 250

// IN
struct PNCTIW_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;

    float4 i : INDEX;
    float4 i2 : SECONDI;
    float4 w : WEIGHT;
    float4 w2 : SECONDW;
};

// Constant
cbuffer AnimationBuffer : register(b5)
{
    matrix obj_matAnim[MAX_BONE];
}

cbuffer InverseBoneBuffer : register(b9)
{
    matrix obj_matBone[MAX_BONE];
}