#include "Header.hlsli"
#include "SkinningHeader.hlsli"

VS_OUT_RIM VS(PNCTIW_IN input)
{
    VS_OUT_RIM output;
    output.p = float4(input.p, 1.0f);
    float4 vLocal = float4(input.p, 1.0f);
    
    float4 vAnim;
       
    for (int i = 0; i < 4; i++)
    {
        int iBoneIndex = (int) input.i[i];
        float fWeight = input.w[i];
        
        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame);
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    for (int j = 0; j < 4; j++)
    {
        int iBoneIndex = (int) input.i2[j];
        float fWeight = input.w2[j];

        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame);
        
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    float4 vWorld = mul(vAnim, g_matWorld);
    float4 vView = mul(vWorld, g_matView);
    float4 vProj = mul(vView, g_matProj);
        
    output.p = vProj;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.wPos = worldPos.xyz;
    output.n = normalize(mul(float4(input.n, 0.0f), g_matWorld).xyz);
    
    output.c = input.c;
    //output.n = input.n;
    output.t = input.t;
    
    return output;
}

VS_OUT VS_SHADOW(PNCTIW_IN input)
{
    VS_OUT output;
    //float4 worldPos = mul(float4(input.p, 1.0f), g_matShadowWorld);
    //worldPos = mul(worldPos, g_matShadowView);
    //output.p = mul(worldPos, g_matShadowProj);
    
    
    output.p = float4(input.p, 1.0f);
    float4 vLocal = float4(input.p, 1.0f);
    
    float4 vAnim;
       
    for (int i = 0; i < 4; i++)
    {
        int iBoneIndex = (int) input.i[i];
        float fWeight = input.w[i];
        
        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame);
        
        animMat = mul(animMat, boneMat);
        animMat = transpose(animMat);
        
        vAnim += mul(vLocal, animMat) * fWeight;
    }
    
    for (int j = 0; j < 4; j++)
    {
        int iBoneIndex = (int) input.i2[j];
        float fWeight = input.w2[j];

        matrix boneMat = obj_matBone[iBoneIndex];
        matrix animMat = GetAnimMatrix(track, iBoneIndex, frame);
        
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


PS_OUT PS(VS_OUT_RIM input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;
    
    if (g_iRenderMode == 1)
    {
        float4 silhouette = float4(0.2f, 0.2f, 0.2f, 1.0f);
        output.c = silhouette;
        output.c1 = silhouette;
        output.c2 = silhouette;
        return output;
    }

    float4 texColor = g_txDiffuseA.Sample(sample, input.t);
    float4 crashColor = g_txCrack.Sample(sample, input.t);
    float dissolve = g_txNoise.Sample(sample, input.t).r;
    
    if (dissolve < g_fDissolveAmount)
        discard;
    
    float3 emissive;
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 ambient = ApplyAmbient();
    float3 flash;

    if (texColor.r < 0.6f)
    {
        emissive = float3(0, 0, 0);
        ambient = float3(1, 1, 1);
        specular = float3(0, 0, 0);
        output.c2 = float4(0.f, 0.f, 1.f, 1.f);
    }
    else
    {
        emissive = g_vEmissiveColor * g_fEmissivePower * 5.0f;
        output.c2 = float4(1.f, 1.f, 1.f, 1.f);
    }
    
    float3 baseColor = texColor.rgb;
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    

    float3 litColor = baseColor * (ambient + diffuse) + specular;

    
    float4 finalColor = float4(litColor + emissive, texColor.a);
    finalColor.rgb = ApplyGlow(finalColor.rgb);

    // output.c : ���� ���
    //output.c = float4(finalColor);

    // output.c1 : Blur ���� (���� ���� ����� �����ϰ� �ص� ����)
    

    float t = 1.0f - g_fHitFlashTime; // 0 �� 1 (�ð� �帧 ����)

    float wave = sin(t * 20.0f * 3.14159); // 10ȸ ����
    float decay = exp(-t * 4.0f); // ���� ���� (���� ����)

    float strength = saturate(abs(wave) * decay);
    float3 flashColor = float3(1.0f, 0.1f, 0.1f); // ���� ������

    finalColor.rgb = lerp(finalColor.rgb, flashColor, strength);
    if (g_bCrash)
    {
        float3 crackHighlightColor = float3(1.0f, 0.1f, 0.1f); // ���� ����
        finalColor.rgb = lerp(finalColor.rgb, crackHighlightColor, crashColor.a);
    }
    
   
    output.c = float4(finalColor);
    output.c1 = float4(finalColor);

    //output.c3 = float4(flashColor, flashAlpha);
    
    output.c4.rgb = (ambient + diffuse) + specular;
    output.c4.a = 1.f;
    output.c5.rgb = input.n;
    output.c5.a = 1.f;
    output.c6 = float4(input.p.z / input.p.w, 0.0f, 0.0f, 1.0f); // R ä�ο��� ����
    

    return output;
}

float PS_SHADOW(VS_OUT input) : SV_Depth
{
    return input.p.z / input.p.w;
}