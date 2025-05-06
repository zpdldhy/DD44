#include "Header.hlsli"

VS_OUT VS(PNCTIW_IN input)
{
    VS_OUT output;
    output.p = float4(input.p, 1.0f);
    float4 vLocal = float4(input.p, 1.0f);
    
    float4 vAnim;
    

    
    for (int i = 0; i < 4; i++)
    {
        int iBoneIndex = (int) input.i[i];
        float fWeight = input.w[i];
        
        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = obj_matAnim[iBoneIndex];
        
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    for (int j = 0; j < 4; j++)
    {
        int iBoneIndex = (int) input.i2[j];
        float fWeight = input.w2[j];

        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = obj_matAnim[iBoneIndex];
        
        animMat = mul(boneMat, animMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    float4 vWorld = mul(vAnim, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
        
    output.p = vProj;
    output.c = input.c;
    output.n = input.n;
    output.t = input.t;
    
    return output;
}

PS_OUT PS(VS_OUT input)
{
    PS_OUT psOut = (PS_OUT) 0;
    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    texColor *= input.c;
    psOut.c = texColor;
    //psOut.c = input.c;
    return psOut;
}