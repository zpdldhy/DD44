#include "Header.hlsli"
#include "SkinningHeader.hlsli"



VS_OUT VS(PNCTIW_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    //float4 worldPos = mul(float4(input.p, 1.0f), g_matShadowWorld);
    //worldPos = mul(worldPos, g_matShadowView);
    //output.p = mul(worldPos, g_matShadowProj);
    
    
    output.p = float4(input.p, 1.0f);
    float4 vLocal = float4(input.p, 1.0f);
    
    float4 vAnim = float4(0.f, 0.f, 0.f, 0.f);
       
    for (int i = 0; i < 4; i++)
    {
        int iBoneIndex = (int) input.i[i];
        float fWeight = input.w[i];
        
        matrix boneMat = obj_matBone[iBoneIndex];
        float4 tempRoot = { 0, 0, 0, 0 };
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame, tempRoot);
        
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    for (int j = 0; j < 4; j++)
    {
        int iBoneIndex = (int) input.i2[j];
        float fWeight = input.w2[j];

        float4 tempRoot = { 0, 0, 0, 0 };
        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame, tempRoot);
        
        animMat = mul(boneMat, animMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    
    
    float4 vWorld = mul(vAnim, g_matWorld);
    float4 vView = mul(vWorld, g_matShadowView);
    float4 vProj = mul(vView, g_matShadowProj);
        
    output.p = vProj;
    
    return output;
}


float PS(VS_OUT input) : SV_Depth
{
    return input.p.z / input.p.w;
}