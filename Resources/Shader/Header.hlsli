#ifndef COMMON_HLSLI
#define COMMON_HLSLI
#define MAX_BONE 250
#define MAX_LIGHTS 4

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
    float g_fGlowPower; // �߱� ����
    float3 padding_glow;
    float3 g_vGlowColor; // �߱� ����
    float dummy_glow;
    float g_fHitFlashTime;
}
cbuffer cbDissolve : register(b3)
{
    float g_fDissolveThreshold;
}
cbuffer CB_UVDistortion : register(b4)
{
    float g_fDistortionStrength; // �ְ� ���� (��: 0.01 ~ 0.1)
    float g_fWaveSpeed; // �ð� �帧 �ӵ� (��: 1.0)
    float g_fWaveFrequency; // ���� (��: 10.0)
    float g_fDistortionTime; // ���� �ð� (�����Ӹ��� ������)
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

struct LightData
{
    float4 vLightColor_Intensity; // (RGB, Intensity)
    float4 vLightDirection_Range; // (XYZ, Range)
    float4 vLightPosition_Angle; // (XYZ, Angle)
    float4 vType_AmbientColor; // (iLightType, Ambient RGB)
    float4 vAmbientPower_Padding; // (AmbientPower, padding)
};

cbuffer CB_LightArray : register(b8)
{
    LightData g_arrLights[MAX_LIGHTS];
    int g_iNumLights;
    float3 padding_light;
}


cbuffer InverseBoneBuffer : register(b9)
{
    matrix obj_matBone[MAX_BONE];
}

cbuffer CB_Emissive : register(b10)
{
    float3 g_vEmissiveColor;
    float g_fEmissivePower;
}

cbuffer CB_Blur : register(b11)
{
    float2 g_vTexelSize; // (1 / ȭ�� �ʺ�, 1 / ȭ�� ����)
    float2 g_vDirection; // (1, 0): ���� �� / (0, 1): ���� ��
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
    float4 c : SV_Target0;
    float4 c1 : SV_Target1;
    float4 c2 : SV_Target2;
};

Texture2D g_txDiffuseA : register(t0);
//Texture2D g_txNoise : register(t1); // ������ �ؽ�ó
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

float3 UnpackLightColor(int index)
{
    return g_arrLights[index].vLightColor_Intensity.rgb;
}
float UnpackIntensity(int index)
{
    return g_arrLights[index].vLightColor_Intensity.a;
}
float3 UnpackLightDirection(int index)
{
    return g_arrLights[index].vLightDirection_Range.rgb;
}
float UnpackRange(int index)
{
    return g_arrLights[index].vLightDirection_Range.a;
}
float3 UnpackLightPosition(int index)
{
    return g_arrLights[index].vLightPosition_Angle.rgb;
}
float UnpackAngle(int index)
{
    return g_arrLights[index].vLightPosition_Angle.a;
}
int UnpackType(int index)
{
    return (int) g_arrLights[index].vType_AmbientColor.x;
}
float3 UnpackAmbientColor(int index)
{
    return g_arrLights[index].vType_AmbientColor.yzw;
}
float UnpackAmbientPower(int index)
{
    return g_arrLights[index].vAmbientPower_Padding.x;
}

float3 ApplyLambertLighting(float3 normal, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < g_iNumLights; ++i)
    {
        float3 lightDir;
        float attenuation = 1.0f;

        int type = UnpackType(i);
        if (type == 0)
        {
            lightDir = normalize(-UnpackLightDirection(i));
        }
        else
        {
            float3 toLight = UnpackLightPosition(i) - worldPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);
            attenuation = saturate(1.0 - dist / UnpackRange(i));

            if (type == 2)
            {
                float3 spotDir = normalize(-UnpackLightDirection(i));
                float spotFactor = dot(lightDir, spotDir);
                float spotCos = cos(radians(UnpackAngle(i)));
                float spotEffect = smoothstep(spotCos, spotCos + 0.1f, spotFactor);
                attenuation *= spotEffect;
            }
        }

        float NdotL = saturate(dot(normal, lightDir));
        result += UnpackLightColor(i) * NdotL * UnpackIntensity(i) * attenuation;
    }
    return result;
}

float3 ApplySpecular(float3 normal, float3 worldPos)
{
    float3 result = float3(0, 0, 0);
    for (int i = 0; i < g_iNumLights; ++i)
    {
        float3 lightDir;
        float attenuation = 1.0f;

        int type = UnpackType(i);
        if (type == 0)
        {
            lightDir = normalize(-UnpackLightDirection(i));
        }
        else
        {
            float3 toLight = UnpackLightPosition(i) - worldPos;
            float dist = length(toLight);
            lightDir = normalize(toLight);
            attenuation = saturate(1.0 - dist / UnpackRange(i));

            if (type == 2)
            {
                float3 spotDir = normalize(-UnpackLightDirection(i));
                float spotFactor = dot(lightDir, spotDir);
                float spotCos = cos(radians(UnpackAngle(i)));
                float spotEffect = smoothstep(spotCos, spotCos + 0.1f, spotFactor);
                attenuation *= spotEffect;
            }
        }

        float3 N = normalize(normal);
        float3 V = normalize(g_vCameraPos - worldPos);
        float3 R = reflect(-lightDir, N);

        float spec = pow(saturate(dot(R, V)), g_fShininess);
        result += UnpackLightColor(i) * spec * UnpackIntensity(i) * attenuation * 10.0f;
    }
    return result;
}

float3 ApplyAmbient()
{
    return UnpackAmbientColor(0) * UnpackAmbientPower(0);
}

#endif