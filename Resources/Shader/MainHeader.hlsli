#ifndef COMMON_HLSLI
#define COMMON_HLSLI

cbuffer cb0 : register(b0)
{
    row_major matrix g_matWorld;
}

cbuffer CameraBuffer : register(b1)
{
    row_major matrix g_matView;
    row_major matrix g_matProj;
    matrix g_matShadowView;
    matrix g_matShadowProj;
    float3 g_vCameraPos;
    float g_fGameTime;
};

cbuffer CB_MaterialEffect : register(b2)
{
    // Glow + HitFlash
    float g_fGlowPower;
    float3 padding_glow;
    float3 g_vGlowColor;
    float g_fHitFlashTime;

    // UV Distortion
    float g_fDistortionStrength;
    float g_fWaveSpeed;
    float g_fWaveFrequency;
    float g_fDistortionTime;

    // Emissive
    float3 g_vEmissiveColor; // 자발광 색상
    float g_fEmissivePower;
    
    // [Light 반응 계수]
    float3 g_vAmbientCoeff; // 주변광 반응도
    float g_fAmbientPower;

    float3 g_vDiffuseCoeff; // 난반사 반응도
    float g_fDiffusePower;

    float3 g_vSpecularCoeff; // 정반사 색상
    float g_fShininess;
    
    float4 g_vTintColor;
    
    bool g_bCrash = false;
    float3 padding_Crash;
};

struct VS_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
};

struct VS_OUT
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
};

struct PS_OUT
{
    float4 c : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
    float4 c3 : SV_Target3;
    float4 c4 : SV_Target4;
    float4 c5 : SV_Target5;
    float4 c6 : SV_Target6;
    float4 c7 : SV_Target7;
};

Texture2D g_txDiffuseA : register(t0);
Texture2D g_txCrack : register(t1);
SamplerState sample : register(s0);
// 그림자용
Texture2D<float> g_txShadow : register(t10); 
SamplerComparisonState g_samShadow : register(s1); 

///funtion
//==============================
// [FX] Shared Utility Functions
//==============================


float2 GetDistortedUV(float2 uv)
{
    float waveX = sin(uv.y * g_fWaveFrequency + g_fDistortionTime * g_fWaveSpeed);
    float waveY = cos(uv.x * g_fWaveFrequency + g_fDistortionTime * g_fWaveSpeed);
    return uv + float2(waveX, waveY) * g_fDistortionStrength;
}

bool ShouldDissolve(float2 distortedUV)
{
    //float noise = g_txNoise.Sample(sample, distortedUV).r;
    //return noise < g_fDissolveThreshold;
    return false;
}

float3 ApplyGlow(float3 baseColor)
{
    float3 glow = g_vGlowColor.rgb * g_fGlowPower;
    float3 toneMapped = (baseColor + glow) / (baseColor + glow + 1.0f);
    float3 softGlow = lerp(baseColor, toneMapped, saturate(g_fGlowPower));
    float glowFactor = saturate((g_fGlowPower - 1.0f) * 0.5f);
    float3 strongGlow = g_vGlowColor.rgb * glowFactor;
    return lerp(softGlow, strongGlow, glowFactor);
}

float3 ApplyRimLight(float3 normal, float3 worldPos)
{
    float3 N = normalize(normal);
    //float3 N = normalize(mul(normal, (float3x3) g_matWorld));
    float3 V = normalize(g_vCameraPos - worldPos);
    float rim = pow(1.0 - saturate(dot(N, V)), 1.0);
    float rimMask = smoothstep(0.2, 0.8, rim);
    return float3(1.0, 1.0, 1.0) * rimMask * 1.5;
}

float3 ApplyHitFlash(float3 baseColor)
{
    return lerp(baseColor, float3(1.0f, 1.0f, 1.0f), saturate(g_fHitFlashTime));
}

float4 ApplyHitFlash(float4 color)
{
    float hitStrength = saturate(g_fHitFlashTime); // 예: 0.0 ~ 1.0
    float3 flashColor = lerp(color.rgb, float3(1, 1, 1), hitStrength);
    float alpha = lerp(color.a, 0.8f, hitStrength); // 알파도 줄이기
    return float4(flashColor, alpha);
}

float3 TransformHSV(
    float3 inputCol, // color to transform
    float h, // 색조:hue shift (in degrees) : 
    float s, // 채도:saturation multiplier (scalar)
    float v // 색상전체의 밝기를 조정:value multiplier (scalar)
)
{
    float vsu = v * s * cos(h * 3.141592 / 180.0);
    float vsw = v * s * sin(h * 3.141592 / 180.0);
    
    // RGB 변환
    float3 ret = float3(0, 0, 0);
    ret.r = (.299 * v + .701 * vsu + .168 * vsw) * inputCol.r
        + (.587 * v - .587 * vsu + .330 * vsw) * inputCol.g
        + (.114 * v - .114 * vsu - .497 * vsw) * inputCol.b;
    ret.g = (.299 * v - .299 * vsu - .328 * vsw) * inputCol.r
        + (.587 * v + .413 * vsu + .035 * vsw) * inputCol.g
        + (.114 * v - .114 * vsu + .292 * vsw) * inputCol.b;
    ret.b = (.299 * v - .300 * vsu + 1.25 * vsw) * inputCol.r
        + (.587 * v - .588 * vsu - 1.05 * vsw) * inputCol.g
        + (.114 * v + .886 * vsu - .203 * vsw) * inputCol.b;
    
    return ret;
}

#endif