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

// INSTANCE IW IN
struct INSTANCEIW_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;

    float4 i : INDEX;
    float4 i2 : SECONDI;
    float4 w : WEIGHT;
    float4 w2 : SECONDW;
    
    row_major float4x4 matWorld : TRANSFORM;
    float4 color : INSTANCE_COLOR;
    float4 uv : INSTANCE_UV;
    
    uint track : TRACK;
    uint frame : FRAME;
    float4 rootPos : ROOTPOS;
};

// Skinning PS IN
struct VS_OUT_RIM_INSTANCE
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
    float3 wPos : POSITIONWS;
    float4 shadowCoord : TEXCOORD1;
    float4 color : INSTANCE_COLOR;
};

// Constant
cbuffer AnimationBuffer : register(b5)
{
    uint track;
    uint frame;
    uint p1;
    uint p2;
    float4 rootPos;
}

cbuffer InverseBoneBuffer : register(b9)
{
    matrix obj_matBone[MAX_BONE];
}

//
Texture3D<float4> g_AnimTex : register(t3);

// Helper Func
float4x4 GetAnimMatrix(uint _track, uint _bone, uint _frame, float4 _root)
{
    // Texture3D (X: bone, Y: frame, Z: track * 4 + row index)
    float4 row0 = g_AnimTex.Load(int4(_bone, _frame, _track * 4 + 0, 0));
    float4 row1 = g_AnimTex.Load(int4(_bone, _frame, _track * 4 + 1, 0));
    float4 row2 = g_AnimTex.Load(int4(_bone, _frame, _track * 4 + 2, 0));
    float4 row3 = g_AnimTex.Load(int4(_bone, _frame, _track * 4 + 3, 0));

    float4x4 mat = float4x4(
        row0,
        row1,
        row2,
        row3
    );
    float4x4 root = float4x4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    -_root.x, -_root.y, -_root.z, 1);

    mat = mul(mat, root);
    return transpose(mat);
}