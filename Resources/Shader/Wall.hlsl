#include "Header.hlsli"

VS_OUT_RIM VS(VS_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    output.wPos = worldPos.xyz;
    
    float3x3 worldMat3 = (float3x3) g_matWorld;
    output.n = normalize(mul(input.n, worldMat3));
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.c = input.c;
    output.t = input.t;

    float4 shadowCoord = mul(mul(worldPos, g_matShadowView), g_matShadowProj);
    shadowCoord.y = -shadowCoord.y;
    output.shadowCoord = shadowCoord;
    
    
    return output;
}

VS_OUT_RIM VS_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT_RIM output = (VS_OUT_RIM) 0;

    float4 worldPos = mul(float4(input.p, 1.0f), input.matWorld);
    output.wPos = worldPos.xyz;
    
    float3x3 worldMat3 = (float3x3) input.matWorld;
    output.n = normalize(mul(input.n, worldMat3));
    output.p = mul(mul(worldPos, g_matView), g_matProj);
    output.c = input.c;
    output.t = input.t;
    
    float4 shadowCoord = mul(mul(worldPos, g_matShadowView), g_matShadowProj);
    shadowCoord.y = -shadowCoord.y;
    output.shadowCoord = shadowCoord;

    return output;
}


VS_OUT VS_SHADOW(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.p, 1.0f), g_matWorld);
    worldPos = mul(worldPos, g_matShadowView);
    output.p = mul(worldPos, g_matShadowProj);
    
    return output;
}

VS_OUT VS_SHADOW_INSTANCE(VS_INSTANCE_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    float4 worldPos = mul(float4(input.p, 1.0f), input.matWorld);
    worldPos = mul(worldPos, g_matShadowView);
    output.p = mul(worldPos, g_matShadowProj);
    
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
    float3 baseColor = texColor.rgb;
    
    float3 shadowTexCoord = input.shadowCoord.xyz / input.shadowCoord.w;
    shadowTexCoord.xy = shadowTexCoord.xy * 0.5f + 0.5f;

    float shadow = 1.0f;
    if (shadowTexCoord.x < 0 || shadowTexCoord.x > 1 || shadowTexCoord.y < 0 || shadowTexCoord.y > 1)
    {
        shadow = 1.0f;
    }
    else
    {
        float2 texelSize = float2(1.0f / 8192.0f, 1.0f / 8192.0f); // 
        float result = 0.0f;
        
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                float2 offset = float2(x, y) * texelSize;
                result += g_txShadow.SampleCmpLevelZero(g_samShadow, shadowTexCoord.xy + offset, shadowTexCoord.z - 0.0001f);
            }
        }
        shadow = result / 9.0f;
        
        shadow = lerp(1.0f, shadow, 0.5f);
    }
    
    float3 ambient = ApplyAmbient();
    float3 diffuse = ApplyLambertLighting(input.n, input.wPos);
    float3 specular = ApplySpecular(input.n, input.wPos);
    float3 litColor = baseColor * (ambient + diffuse) + specular;
    litColor *= shadow;
    float3 emissive = g_vEmissiveColor * g_fEmissivePower;
    float3 finalColor = litColor + emissive;
    finalColor = ApplyGlow(finalColor);
    finalColor = ApplyHitFlash(finalColor);


    output.c = float4(finalColor, texColor.a);
    output.c1 = float4(finalColor, texColor.a);
    output.c2 = float4(0.f, 0.f, 1.f, 1.f); 
    
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