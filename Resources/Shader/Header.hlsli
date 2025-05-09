#ifndef COMMON_HLSLI
#define COMMON_HLSLI
#define MAX_BONE 250

static const float g_fShininess = 32.0f;

cbuffer cb0 : register(b0)
{
    row_major matrix g_matWorld;
}

cbuffer CameraBuffer : register(b1)
{
    row_major matrix g_matView;
    row_major matrix g_matProj;
};

cbuffer cbGlowFX : register(b2)
{
    float g_fGlowPower; // 발광 세기
    float3 padding_glow;
    float3 g_vGlowColor; // 발광 색상
    float dummy_glow;
    float g_fHitFlashTime;
}
cbuffer cbDissolve : register(b3)
{
    float g_fDissolveThreshold;
}
cbuffer CB_UVDistortion : register(b4)
{
    float g_fDistortionStrength; // 왜곡 세기 (예: 0.01 ~ 0.1)
    float g_fWaveSpeed; // 시간 흐름 속도 (예: 1.0)
    float g_fWaveFrequency; // 파장 (예: 10.0)
    float g_fDistortionTime; // 누적 시간 (프레임마다 누적됨)
};

cbuffer AnimationBuffer : register(b5)
{
    matrix obj_matAnim[MAX_BONE];
}
cbuffer CB_Camera : register(b6)
{
    float3 g_vCameraPos;
    float dummy_camera; 
};

cbuffer CB_RenderMode : register(b7)
{
    int g_iRenderMode;
    float3 padding_rendermode;
};

cbuffer CB_Light : register(b8)
{
    float3 g_vLightColor;
    float g_fIntensity;

    float3 g_vLightDirection;
    float g_fRange;

    float3 g_vLightPosition;
    float g_fAngle;

    int g_iLightType; // 0 = Dir, 1 = Point, 2 = Spot
    float3 g_vAmbientColor;

    float g_fAmbientPower; // 
    float3 padding_light; // 
};

cbuffer InverseBoneBuffer : register(b9)
{
    matrix obj_matBone[MAX_BONE];
}

cbuffer CB_Emissive : register(b10)
{
    float3 g_vEmissiveColor;
    float g_fEmissivePower;
}

cbuffer CB_Material : register(b11)
{
    float4 g_vMaterialAmbient; // 물체의 ambient 반사율
    float4 g_vMaterialDiffuse; // 물체의 diffuse 반사율
    float4 g_vMaterialSpecular; // 물체의 specular 반사율
    float4 g_vMaterialEmissive; // 물체의 emissive 자발광 색
};

struct VS_IN
{
    float3 p : POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
};

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

struct VS_OUT
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
};

struct VS_OUT_RIM
{
    float4 p : SV_POSITION;
    float4 c : COLOR;
    float3 n : NORMAL;
    float2 t : TEXCOORD;
    float3 wPos : POSITIONWS;
};

struct PS_OUT
{
    float4 c : SV_Target;
};

Texture2D g_txDiffuseA : register(t0);
Texture2D g_txNoise : register(t1); // 노이즈 텍스처
SamplerState sample : register(s0);


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
    float noise = g_txNoise.Sample(sample, distortedUV).r;
    return noise < g_fDissolveThreshold;
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

float3 ApplyLambertLighting(float3 normal)
{
    float3 lightDir = normalize(-g_vLightDirection);
    float NdotL = saturate(dot(normal, lightDir));
    //return g_vLightColor * NdotL * g_fIntensity;
    return g_vLightColor * g_vMaterialDiffuse.rgb * NdotL * g_fIntensity;
}

float3 ApplySpecular(float3 normal, float3 worldPos)
{
    float3 N = normalize(normal);
    float3 L = normalize(-g_vLightDirection);
    float3 V = normalize(g_vCameraPos - worldPos);
    //float3 H = normalize(L + V); // Half vector
    float3 R = reflect(-L, N); // 정반사 벡터 사용 (Phong 모델)

    float spec = pow(saturate(dot(R, V)), g_fShininess);
    // 메탈릭 반짝임 강조
    float3 specularColor = g_vLightColor * g_vMaterialSpecular.rgb * spec * g_fIntensity * 20.0f;

    return specularColor;
}

float3 ApplyAmbient()
{
    //return g_vAmbientColor * g_fAmbientPower;
    return g_vAmbientColor * g_fAmbientPower * g_vMaterialAmbient.rgb;
}

#endif